#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dbg::util
{

/// Convert array of arguments of type string to char*
/// @param args arguments to convert to C-String format
/// @param exePath path to an elf binary (argv[0])
/// @return array of arguments in C-String format
std::vector<char*> toCStringArray(const std::vector<std::string>& args, const std::string& exePath);

/// Get error message after exec using errno
std::string getExecErrnoMessage();

/// Get base address of a running process
/// @param pid currently running process
/// @param exePath path to an elf binary
/// @return base address of the mapped main executable at runtime
uintptr_t getBaseAddress(pid_t pid, const std::string& exePath);

/// Find symbol in and elf file's symtable section
/// @param exePath path to an elf binary
/// @param symbolName symbol name to be extracted from the binary
/// @return link time offset and size of the symbol
std::pair<uintptr_t, size_t> findSymbol(const std::string& exePath, const std::string& symbolName);

/// Set watchpoint for process with pid at address addr
/// @param pid id of process watchpoint will be set to
/// @param addr address of the watchpoint
/// @param size size of the tracked address (bytes)
/// @param type describes when an interrupt is triggered
void setHardwareWatchpoint(pid_t pid, uintptr_t addr, size_t size);

/// Symbolizes in which context did the watchpoint occur
enum WatchpointEvent
{
    READ,
    WRITE,
    OTHER   // any other event (ignored for this task)
};

/// Returns the WatchpointEvent which was a cause of current interrupt,
/// should be executed only once per interrupt, because also clears DR6 debug register
/// @param pid id of the process to check
/// @return WatchpointEvent which was a cause of current interrupt
WatchpointEvent getWatchpointEvent(pid_t pid);

} // namespace dbg::util