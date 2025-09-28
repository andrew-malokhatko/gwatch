#pragma once

#include "Variable.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace dbg
{

class Debugger
{
    std::string m_path;
    std::vector<std::string> m_args;
    Variable m_var;

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

    void setOnRead(callback_t onRead);
    void setOnWrite(callback_t onWrite);
    void setVariable(const Variable& variable);
    void run();
};

} // namespace dbg
