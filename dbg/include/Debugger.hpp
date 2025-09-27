#pragma once

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

    // args: value, variable size
    using callback_t = std::function<void(long, size_t)>;

    callback_t m_onRead;
    callback_t m_onWrite;

    void runDebugger(pid_t childPid, const std::string& watchedVariable);
    void runChild();

  public:
    Debugger(const std::string& program, std::vector<std::string>& args, callback_t onRead = {}, callback_t onWrite = {});

    void setOnRead(callback_t onRead);
    void setOnWrite(callback_t onWrite);
    void run(const std::string& watchedVariable);
};

} // namespace dbg
