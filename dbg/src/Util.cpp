#include "Util.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <unistd.h>

namespace dbg::util
{

namespace fs = std::filesystem;

std::vector<char*> toCStringArray(const std::vector<std::string>& args, const std::string& exePath)
{
    std::vector<char*> cstrArray;
    cstrArray.reserve(args.size() + 2);                      // +1 for program name, +1 for nullptr
    cstrArray.push_back(const_cast<char*>(exePath.c_str())); // argv[0] program name

    for (const auto& arg : args)
    {
        cstrArray.push_back(const_cast<char*>(arg.c_str()));
    }

    cstrArray.push_back(nullptr); // Null-terminate
    return cstrArray;
}

std::string getExecErrnoMessage()
{
    switch (errno)
    {
    case E2BIG: return "execv failed: Argument list too long (E2BIG)";
    case EACCES: return "execv failed: Permission denied (EACCES)";
    case EFAULT: return "execv failed: Invalid address (EFAULT)";
    case EIO: return "execv failed: I/O error (EIO)";
    case ELOOP: return "execv failed: Too many symbolic links (ELOOP)";
    case ENAMETOOLONG: return "execv failed: Path too long (ENAMETOOLONG)";
    case ENOENT: return "execv failed: File does not exist (ENOENT)";
    case ENOEXEC: return "execv failed: Invalid executable format (ENOEXEC)";
    case ENOMEM: return "execv failed: Out of memory (ENOMEM)";
    case ENOTDIR: return "execv failed: Not a directory (ENOTDIR)";
    case ETXTBSY: return "execv failed: Text file busy (ETXTBSY)";
    default: return "execv failed: Unknown error (" + std::string(std::strerror(errno)) + ")";
    }
}

uintptr_t getBaseAddress(pid_t pid, const std::string& exePath)
{
    std::string mapsPath = "/proc/" + std::to_string(pid) + "/maps";
    std::ifstream maps(mapsPath);
    if (!maps)
        throw std::runtime_error("failed to open" + mapsPath);

    std::string line;
    while (std::getline(maps, line))
    {
        std::istringstream iss(line);
        std::string addr, perms, offset, dev, inode, path;
        if (!(iss >> addr >> perms >> offset >> dev >> inode))
            continue;

        std::getline(iss, path); // path may contain spaces
        if (!path.empty() && path.front() == ' ')
        {
            path.erase(0, path.find_first_not_of(' '));
        }

        try
        {
            // use fs::canonical to resolve relative paths
            fs::path exeCanonical = fs::canonical(fs::path(exePath));
            fs::path mapCanonical = fs::canonical(fs::path(path));

            if (exeCanonical == mapCanonical)
            {
                size_t dash = addr.find('-'); // start addr is before '-'
                std::string startAddress = addr.substr(0, dash);
                return std::stoull(startAddress, nullptr, 16);
            }
        }
        catch (std::exception& e) // fs::canonical may throw if path does not exist
        {
        }
    }

    throw std::runtime_error("could not find x permission in mappings for " + exePath);
}

std::pair<uint64_t, uint64_t> findSymbol(const std::string& exePath, const std::string& symbolName)
{
    int fd = open(exePath.c_str(), O_RDONLY);
    if (fd < 0)
    {
        throw std::runtime_error("Could not open " + exePath);
    }

    struct stat st{};
    int ret = fstat(fd, &st);
    if (ret < 0)
    {
        close(fd);
        throw std::runtime_error("Fstat failed: " + exePath);
    }

    void* map = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED)
    {
        close(fd);
        throw std::runtime_error("Mmap failed: " + exePath);
    }

    const char* base = reinterpret_cast<const char*>(map);
    const Elf64_Ehdr* elfHdr = reinterpret_cast<const Elf64_Ehdr*>(base);

    // check elf magic
    if (memcmp(elfHdr->e_ident, ELFMAG, SELFMAG) != 0)
    {
        munmap(map, st.st_size);
        close(fd);
        throw std::runtime_error(exePath + " is not an ELF file");
    }

    // setup section headers
    const Elf64_Shdr* elfSecHdr = reinterpret_cast<const Elf64_Shdr*>(base + elfHdr->e_shoff);
    const Elf64_Shdr& secHdrStrTableHdr = elfSecHdr[elfHdr->e_shstrndx];
    const char* secHdrStrTable = base + secHdrStrTableHdr.sh_offset;

    // find .symtab and .strtab section headers
    const Elf64_Shdr* symtabHdr = nullptr;
    const Elf64_Shdr* strtabHdr = nullptr;
    for (int i = 0; i < elfHdr->e_shnum; ++i)
    {
        const char* name = secHdrStrTable + elfSecHdr[i].sh_name;
        if (strcmp(name, ".symtab") == 0)
            symtabHdr = &elfSecHdr[i];
        if (strcmp(name, ".strtab") == 0)
            strtabHdr = &elfSecHdr[i];
    }

    if (!symtabHdr || !strtabHdr)
    {
        munmap(map, st.st_size);
        close(fd);
        throw std::runtime_error(".symtab or .strtab section was not found: " + exePath);
    }

    // interpret the symbol table as an array of symbols
    const Elf64_Sym* symbols = reinterpret_cast<const Elf64_Sym*>(base + symtabHdr->sh_offset);
    size_t symbolCount = symtabHdr->sh_size / sizeof(Elf64_Sym);
    const char* stringTable = base + strtabHdr->sh_offset;

    // search symbol table entries
    for (size_t i = 0; i < symbolCount; ++i)
    {
        std::string name = stringTable + symbols[i].st_name; // st_name is offset to stringTable
        if (name == symbolName) // ignore c++ name mangling (see README on global variables)
        {
            uint64_t val = symbols[i].st_value; // symbols link-time offset
            uint64_t size = symbols[i].st_size;
            munmap(map, st.st_size);
            close(fd);
            return {val, size};
        }
    }

    munmap(map, st.st_size);
    close(fd);
    throw std::runtime_error("Symbol not found: " + symbolName);
}

/// RW Access type for hardware debug registers
enum AccessType
{
    ON_EXECUTION = 0,
    ON_DATA_WRITE = 1,
    ON_READ_WRITE = 3
};

void setHardwareWatchpoint(pid_t pid, uintptr_t addr, size_t size)
{
    // set address to DR0
    long ret = ptrace(PTRACE_POKEUSER, pid, offsetof(struct user, u_debugreg[0]), reinterpret_cast<void*>(addr));
    if (ret == -1)
    {
        throw std::runtime_error("PTRACE_POKEUSER DR0 failed: " + std::string(strerror(errno)));
    }

    // set address to DR1
    ret = ptrace(PTRACE_POKEUSER, pid, offsetof(struct user, u_debugreg[1]), reinterpret_cast<void*>(addr));
    if (ret == -1)
    {
        throw std::runtime_error("PTRACE_POKEUSER DR0 failed: " + std::string(strerror(errno)));
    }

    // Construct DR7 value (x64 specific)
    // RW0 (bits 16 - 17) = type 00=on exec, 01=on writes, 11=on read and write
    // LEN0 (bits 18-19) = size encoding 00=1, 01=2, 10=8, 11=4
    unsigned int lenEncoding;
    switch (size)
    {
    case 1: lenEncoding = 0; break;
    case 2: lenEncoding = 1; break;
    case 4: lenEncoding = 3; break;
    case 8: lenEncoding = 2; break;
    default: throw std::runtime_error("Invalid watchpoint size " + std::to_string(size));
    }

    uint64_t dr7 = 0;

    dr7 |= 1ULL << 0;                                  // enable local L0
    dr7 |= static_cast<uint64_t>(ON_DATA_WRITE) << 16; // set RW0 bits to write only
    dr7 |= static_cast<uint64_t>(lenEncoding) << 18;   // set LEN0 bits

    dr7 |= 1ULL << 2;                                  // enable local L1
    dr7 |= static_cast<uint64_t>(ON_READ_WRITE) << 20; // set RW1 bits to read write
    dr7 |= static_cast<uint64_t>(lenEncoding) << 22;   // set LEN1 bits

    ret = ptrace(PTRACE_POKEUSER, pid, offsetof(struct user, u_debugreg[7]), reinterpret_cast<void*>(dr7));
    if (ret == -1)
    {
        throw std::runtime_error("PTRACE_POKEUSER DR7 failed: " + std::string(strerror(errno)));
    }
}

WatchpointEvent getWatchpointEvent(pid_t pid)
{
    long reg = ptrace(PTRACE_PEEKUSER, pid, offsetof(struct user, u_debugreg[6]), nullptr);
    if (reg == -1 && errno != 0)
    {
        throw std::runtime_error("PTRACE_PEEKUSER DR6 failed: " + std::string(strerror(errno)));
    }

    uint64_t reg0 = reg & (1ULL << 0); // write only
    uint64_t reg1 = reg & (1ULL << 1); // read and write

    // clear the debug register
    long ret = ptrace(PTRACE_POKEUSER, pid, offsetof(struct user, u_debugreg[6]), 0);
    if (ret != 0)
    {
        throw std::runtime_error("PTRACE_POKEUSER DR6 failed: " + std::string(strerror(errno)));
    }

    if (reg0 && reg1)
    {
        return WRITE;
    }
    if (!reg0 && reg1)
    {
        return READ;
    }

    return OTHER;
}

} // namespace dbg::util