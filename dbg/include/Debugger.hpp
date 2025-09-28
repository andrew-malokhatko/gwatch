#pragma once

#include "Variable.hpp"

#include <functional>
#include <string>
#include <vector>

namespace dbg
{

class Debugger
{
    std::string m_path;
    std::vector<std::string> m_args;
    Variable m_var;
    Variable m_prevVar{};

    // args: value, variable size
    using callback_t = std::function<void(const Variable&)>;

    callback_t m_onRead;
    callback_t m_onWrite;

private:
    void trackNewThread(pid_t threadId);

    void attachDebugger(pid_t childPid);
    void traceChild(pid_t childPid);

    void runChild();

  public:
    Debugger(const std::string& program, const std::vector<std::string>& args, const Variable& variable);

    Debugger(const Debugger&) = delete;
    Debugger(Debugger&&) = delete;
    Debugger& operator=(const Debugger&) = delete;
    Debugger& operator=(Debugger&&) = delete;

    void setOnRead(callback_t onRead);
    void setOnWrite(callback_t onWrite);

    const Variable& getVar() const;
    const Variable& getLastVar() const;

    void run();
};

} // namespace dbg
