#include "Debugger.hpp"
#include <gtest/gtest.h>

/// Functional tests for Debugger class
class DebuggerTests : public ::testing::Test
{
  protected:
    const std::string HELLO_WORLD_PATH = "./hello_world";
    const std::string ARG_PRINTER_PATH = "./arg_printer";
    const std::string FACTORIAL_PATH = "./factorial";
    const std::string ONE_READ_PATH = "./one_read";
    const std::string ONE_READ_SHORT_PATH = "./one_read_short";
    const std::string ONE_READ_LONG_PATH = "./one_read_long";
    const std::string ONE_WRITE_PATH = "./one_write";
};

TEST_F(DebuggerTests, ExecHelloWorld)
{
    std::vector<std::string> args{};
    dbg::Debugger debugger(HELLO_WORLD_PATH, args);

    debugger.run("global_var");
}

TEST_F(DebuggerTests, ExecWithParameters)
{
    std::vector<std::string> args{"Executed", "with", "parameters"};
    dbg::Debugger debugger(ARG_PRINTER_PATH, args);

    debugger.run("global_var");
}

TEST_F(DebuggerTests, OneRead)
{
    std::vector<std::string> args{};
    dbg::Debugger debugger(ONE_READ_PATH, args);

    std::vector<int> read;
    std::vector<int> write;

    debugger.setOnRead([&read](long val, size_t size) {
        read.push_back(static_cast<int>(val));
        ASSERT_EQ(size, sizeof(int));
    });

    debugger.setOnWrite([&write](long val, size_t size) { write.push_back(static_cast<int>(val)); });

    debugger.run("global_var");

    ASSERT_EQ(read.size(), 1);
    ASSERT_TRUE(write.empty());

    ASSERT_EQ(read[0], 42);
}

TEST_F(DebuggerTests, OneReadShort)
{
    std::vector<std::string> args{};
    dbg::Debugger debugger(ONE_READ_SHORT_PATH, args);

    std::vector<short> read;
    std::vector<short> write;

    debugger.setOnRead([&read](long val, size_t size) {
        read.push_back(static_cast<short>(val));
        ASSERT_EQ(size, sizeof(short));
    });

    debugger.setOnWrite([&write](long val, size_t size) { write.push_back(static_cast<short>(val)); });

    debugger.run("global_var");

    ASSERT_EQ(read.size(), 1);
    ASSERT_TRUE(write.empty());

    ASSERT_EQ(read[0], 42);
}

TEST_F(DebuggerTests, OneReadLong)
{
    std::vector<std::string> args{};
    dbg::Debugger debugger(ONE_READ_LONG_PATH, args);

    std::vector<short> read;
    std::vector<short> write;

    debugger.setOnRead([&read](long val, size_t size) {
        read.push_back(val);
        ASSERT_EQ(size, sizeof(long));
    });

    debugger.setOnWrite([&write](long val, size_t size) { write.push_back(static_cast<short>(val)); });

    debugger.run("global_var");

    ASSERT_EQ(read.size(), 1);
    ASSERT_TRUE(write.empty());

    ASSERT_EQ(read[0], 42);
}

TEST_F(DebuggerTests, OneWrite)
{
    std::vector<std::string> args{};
    dbg::Debugger debugger(ONE_WRITE_PATH, args);

    std::vector<long> read;
    std::vector<long> write;

    debugger.setOnRead([&read](long val, size_t size) {
        read.push_back(val);
        ASSERT_EQ(size, sizeof(long));
    });

    debugger.setOnWrite([&write](long val, size_t size) {
        write.push_back(val);
        ASSERT_EQ(size, sizeof(long));
    });

    debugger.run("global_var");

    ASSERT_EQ(write.size(), 1);
    ASSERT_EQ(read.size(), 0);

    ASSERT_EQ(write[0], 142);
}