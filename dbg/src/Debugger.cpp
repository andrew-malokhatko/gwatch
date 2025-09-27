#include "Debugger.hpp"

#include "Util.hpp"

#include <cstring>
#include <stdexcept>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

namespace dbg
{

Debugger::Debugger(const std::string& program, const std::vector<std::string>& args, callback_t onRead, callback_t onWrite)
    : m_path{program},
      m_args{args},
      m_onRead{onRead},
      m_onWrite{onWrite}
{
}

void Debugger::setOnRead(std::function<void(long, size_t)> onRead)
{
    m_onRead = onRead;
}

void Debugger::setOnWrite(std::function<void(long, size_t)> onWrite)
{
    m_onWrite = onWrite;
}

void Debugger::runDebugger(pid_t childPid, const std::string& watchedVariable)
{
    int status = 0;
    pid_t wRet = waitpid(childPid, &status, 0);
    if (wRet < 0)
    {
        throw std::runtime_error("waitpid failed for " + std::to_string(childPid));
    }

    if (!WIFSTOPPED(status))
    {
        throw std::runtime_error("child did not stop as expected " + std::to_string(childPid));
    }

    // find base address of the process after it was mapped into memory
    uintptr_t base = util::getBaseAddress(childPid, m_path);

    // extract symbol information from the elf file
    auto symbol = util::findSymbol(m_path, watchedVariable);
    uintptr_t symbolAddress = base + symbol.first;
    size_t symbolSize = symbol.second;

    // set hardware watchpoint
    util::setHardwareWatchpoint(childPid, symbolAddress, symbolSize);

    long pRet = ptrace(PTRACE_CONT, childPid, nullptr, nullptr);
    if (pRet < 0)
    {
        throw std::runtime_error("PTRACE_CONT failed: " + std::string(strerror(errno)));
    }

    while (true)
    {
        wRet = waitpid(childPid, &status, 0);

        if (wRet < 0)
        {
            if (errno == WNOHANG)
            {
                continue;
            }

            throw std::runtime_error("waitpid failed:" + std::string(strerror(errno)));
        }

        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) != 0)
            {
                std::cerr << "child exited with status " << WEXITSTATUS(status) << "\n";
            }
            break;
        }

        if (WIFSIGNALED(status))
        {
            std::cerr << "child killed by signal " << WTERMSIG(status) << "\n";
            break;
        }

        if (WIFSTOPPED(status))
        {
            // kernel sends SIGTRAP on hardware watchpoint set
            if (WSTOPSIG(status) == SIGTRAP)
            {
                long word = ptrace(PTRACE_PEEKDATA, childPid, symbolAddress, nullptr);
                if (word == -1 && errno != 0)
                {
                    throw std::runtime_error("PTRACE_PEEKDATA failed: " + std::string(strerror(errno)));
                }

                long value = 0;
                memcpy(&value, &word, symbolSize);

                auto watchpointEvent = util::getWatchpointEvent(childPid);
                switch (watchpointEvent)
                {
                case util::READ: m_onRead(value, symbolSize); break;
                case util::WRITE: m_onWrite(value, symbolSize); break;
                }
            }

            pRet = ptrace(PTRACE_CONT, childPid, nullptr, nullptr);
            if (pRet < 0)
            {
                throw std::runtime_error("PTRACE_CONT failed: " + std::string(strerror(errno)));
            }
        }
    }
}

void Debugger::runChild()
{
    std::vector<char*> cStrArray = util::toCStringArray(m_args, m_path);

    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    int ret = execvp(m_path.c_str(), cStrArray.data());

    if (ret == -1)
    {
        // throw std::runtime_error(util::getExecErrnoMessage());
        throw std::runtime_error("Error running " + m_path + " "  + strerror(errno));
    }
}

void Debugger::run(const std::string& watchedVariable)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        throw std::runtime_error("fork failed");
    }

    if (pid != 0)
    {
        runDebugger(pid, watchedVariable);
    }
    else
    {
        runChild();
    }
}

} // namespace dbg