#include "Debugger.hpp"
#include <gtest/gtest.h>

/// Functional tests for Debugger class
class DebuggerTests : public ::testing::Test
{
  protected:
    const std::string ONE_READ_PATH = "./one_read";
    const std::string ONE_READ_SHORT_PATH = "./one_read_short";
    const std::string ONE_READ_LONG_PATH = "./one_read_long";
    const std::string ONE_WRITE_PATH = "./one_write";

    // multithreading
    const std::string READ_THREAD_PATH = "./thread_read";
    const std::string WRITE_THREAD_PATH = "./thread_write";
    const std::string MULTI_THREAD_PATH = "./thread_multi";
};

TEST_F(DebuggerTests, OneRead)
{
    std::vector<std::string> args{};
    dbg::Variable var{"global_var"};
    dbg::Debugger debugger(ONE_READ_PATH, args, var);

    std::vector<int> read;
    std::vector<int> write;

    debugger.setOnRead(
        [&read](const dbg::Variable& var)
        {
            read.push_back(var.get<int>());
            ASSERT_EQ(var.size, sizeof(int));
        });

    debugger.setOnWrite(
        [&write](const dbg::Variable& var)
        {
            write.push_back(var.get<int>());
        });

    debugger.run();

    ASSERT_EQ(read.size(), 1);
    ASSERT_TRUE(write.empty());

    ASSERT_EQ(read[0], 42);
}

TEST_F(DebuggerTests, OneReadShort)
{
    std::vector<std::string> args{};
    dbg::Variable var{"global_var"};
    dbg::Debugger debugger(ONE_READ_SHORT_PATH, args, var);

    std::vector<short> read;
    std::vector<short> write;

    debugger.setOnRead(
        [&read](const dbg::Variable& var)
        {
            read.push_back(var.get<short>());
            ASSERT_EQ(var.size, sizeof(short));
        });

    debugger.setOnWrite(
        [&write](const dbg::Variable& var)
        {
            write.push_back(var.get<short>());
        });

    debugger.run();

    ASSERT_EQ(read.size(), 1);
    ASSERT_TRUE(write.empty());

    ASSERT_EQ(read[0], 42);
}

TEST_F(DebuggerTests, OneReadLong)
{
    std::vector<std::string> args{};
    dbg::Variable var{"global_var"};
    dbg::Debugger debugger(ONE_READ_LONG_PATH, args, var);

    std::vector<long> read;
    std::vector<long> write;

    debugger.setOnRead(
        [&read](const dbg::Variable& var)
        {
            read.push_back(var.get<long>());
            ASSERT_EQ(var.size, sizeof(long));
        });

    debugger.setOnWrite(
        [&write](const dbg::Variable& var)
        {
            write.push_back(var.get<long>());
        });

    debugger.run();

    ASSERT_EQ(read.size(), 1);
    ASSERT_TRUE(write.empty());

    ASSERT_EQ(read[0], 42);
}

TEST_F(DebuggerTests, OneWrite)
{
    std::vector<std::string> args{};
    dbg::Variable var{"global_var"};
    dbg::Debugger debugger(ONE_WRITE_PATH, args, var);

    std::vector<long> read;
    std::vector<long> write;

    debugger.setOnRead(
        [&read](const dbg::Variable& var)
        {
            read.push_back(var.get<long>());
        });

    debugger.setOnWrite(
        [&write](const dbg::Variable& var)
        {
            write.push_back(var.get<long>());
        });

    debugger.run();

    ASSERT_EQ(write.size(), 1);
    ASSERT_EQ(read.size(), 0);

    ASSERT_EQ(write[0], 142);
}

TEST_F(DebuggerTests, ReadThread)
{
    std::vector<std::string> args{};
    dbg::Variable var{"global_var"};
    dbg::Debugger debugger(READ_THREAD_PATH, args, var);

    std::vector<int> read;
    std::vector<int> write;

    debugger.setOnRead(
        [&read](const dbg::Variable& var)
        {
            read.push_back(var.get<int>());
        });

    debugger.setOnWrite(
        [&write](const dbg::Variable& var)
        {
            write.push_back(var.get<int>());
        });

    debugger.run();

    ASSERT_EQ(read.size(), 1);
    ASSERT_TRUE(write.empty());

    ASSERT_EQ(read[0], 42);
}

TEST_F(DebuggerTests, WriteThread)
{
    std::vector<std::string> args{};
    dbg::Variable var{"global_var"};
    dbg::Debugger debugger(WRITE_THREAD_PATH, args, var);

    std::vector<int> read;
    std::vector<int> write;

    debugger.setOnRead(
        [&read](const dbg::Variable& var)
        {
            read.push_back(var.get<int>());
        });

    debugger.setOnWrite(
        [&write](const dbg::Variable& var)
        {
            write.push_back(var.get<int>());
        });

    debugger.run();

    ASSERT_EQ(write.size(), 1);
    ASSERT_TRUE(read.empty());

    ASSERT_EQ(write[0], 142);
}

TEST_F(DebuggerTests, MutiThread)
{
    std::vector<std::string> args{};
    dbg::Variable var{"global_var"};
    dbg::Debugger debugger(MULTI_THREAD_PATH, args, var);

    std::vector<int> read;
    std::vector<int> write;

    debugger.setOnRead(
        [&read](const dbg::Variable& var)
        {
            read.push_back(var.get<int>());
        });

    debugger.setOnWrite(
        [&write](const dbg::Variable& var)
        {
            write.push_back(var.get<int>());
        });

    debugger.run();

    ASSERT_EQ(write.size(), 20000);
    ASSERT_EQ(read.size(), 20000);
}