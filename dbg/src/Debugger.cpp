#include "Debugger.hpp"

#include "Util.hpp"

#include <cassert>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

namespace dbg
{

Debugger::Debugger(const std::string& program, const std::vector<std::string>& args, const Variable& variable)
    : m_path{program},
      m_args{args},
      m_var{variable}
{
}

void Debugger::setOnRead(callback_t onRead)
{
    m_onRead = onRead;
}

void Debugger::setOnWrite(callback_t onWrite)
{
    m_onWrite = onWrite;
}

void Debugger::setVariable(const Variable& variable)
{
    m_var = variable;
}

void Debugger::trackNewThread(pid_t threadId)
{

    int status = 0;
    int wRet = waitpid(threadId, &status, 0);

    if (wRet < 0)
    {
        throw std::runtime_error("waitpid(newThreadId) failed: " + std::string(strerror(errno)));
    }

    util::setHardwareWatchpoint(threadId, m_var.address, m_var.size);

    long pRet = ptrace(PTRACE_CONT, threadId, nullptr, nullptr);
    if (pRet < 0)
    {
        throw std::runtime_error("PTRACE_CONT failed: " + std::string(strerror(errno)));
    }
}

void Debugger::attachDebugger(pid_t childPid)
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
    auto symbol = util::findSymbol(m_path, m_var.name);
    m_var.address = base + symbol.first;
    m_var.size = symbol.second;

    // set hardware watchpoint
    util::setHardwareWatchpoint(childPid, m_var.address, m_var.size);

    // also trace child's threads
    long pRet = ptrace(PTRACE_SETOPTIONS, childPid, nullptr, PTRACE_O_TRACECLONE | PTRACE_O_EXITKILL);
    if (pRet == -1)
    {
        throw std::runtime_error("PTRACE_SETOPTIONS failed " + std::string(strerror(errno)));
    }

    pRet = ptrace(PTRACE_CONT, childPid, nullptr, nullptr);
    if (pRet < 0)
    {
        throw std::runtime_error("PTRACE_CONT failed: " + std::string(strerror(errno)));
    }
}

void Debugger::traceChild(pid_t childPid)
{
    while (true)
    {
        int status = 0;
        pid_t threadId = waitpid(-1, &status, __WALL);

        if (threadId < 0)
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

            if (childPid == threadId)
            {
                break;
            }
        }

        if (WIFSIGNALED(status))
        {
            std::cerr << "child killed by signal " << WTERMSIG(status) << "\n";

            if (childPid == threadId)
            {
                break;
            }
        }

        if (WIFSTOPPED(status))
        {
            // kernel sends SIGTRAP on hardware watchpoint set
            if (WSTOPSIG(status) == SIGTRAP)
            {
                unsigned int event = static_cast<unsigned int>(status) >> 16;

                // handle new threads
                if (event == PTRACE_EVENT_CLONE)
                {
                    pid_t newTid = 0; // threadId of new thread
                    long pRet = ptrace(PTRACE_GETEVENTMSG, threadId, nullptr, &newTid);
                    if (pRet < 0)
                    {
                        throw std::runtime_error("PTRACE_GETEVENTMSG failed: " + std::string(strerror(errno)));
                    }

                    trackNewThread(newTid);
                }
                else
                {
                    long word = ptrace(PTRACE_PEEKDATA, threadId, m_var.address, nullptr);
                    if (word == -1 && errno != 0)
                    {
                        throw std::runtime_error("PTRACE_PEEKDATA failed: " + std::string(strerror(errno)));
                    }

                    memcpy(&m_var.bytes, &word, m_var.size);

                    auto watchpointEvent = util::getWatchpointEvent(threadId);
                    switch (watchpointEvent)
                    {
                    case util::READ: m_onRead(m_var); break;
                    case util::WRITE: m_onWrite(m_var); break;
                    }
                }
            }

            long pRet = ptrace(PTRACE_CONT, threadId, nullptr, nullptr);
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

    try
    {
        m_path = std::filesystem::canonical(m_path).string();
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Failed to resolve path: " << e.what() << "\n";
        std::exit(3);
    }

    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    int ret = execvp(m_path.c_str(), cStrArray.data());

    if (ret == -1)
    {
        std::cerr << util::getExecErrnoMessage() << "\n";
        std::exit(4);
    }
}

void Debugger::run()
{
    pid_t pid = fork();
    if (pid == -1)
    {
        throw std::runtime_error("fork failed");
    }

    if (pid != 0)
    {
        attachDebugger(pid);
        traceChild(pid);
    }
    else
    {
        runChild();
    }
}

} // namespace dbg