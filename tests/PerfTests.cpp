#include "Debugger.hpp"

#include <gtest/gtest.h>
#include <chrono>

/// Performance tests for Debugger class
/// RAW = only read/write accesses
/// REAL = tries to simulate real workload
class PerfTests : public ::testing::TestWithParam<std::tuple<std::string,int>>
{
  protected:
    const std::string RAW_PATH = "./raw";
    const std::string REAL_PATH = "./real";

    std::string m_path{};
    int m_accessCount{};
    double m_directTime{};

    void SetUp() override
    {
        m_path = std::get<0>(GetParam());
        m_accessCount = std::get<1>(GetParam());

        std::string commandStr {m_path + " " + std::to_string(m_accessCount)};

        // Measure direct run time
        auto startDirect = std::chrono::high_resolution_clock::now();
        int ret = std::system(commandStr.c_str());
        auto endDirect = std::chrono::high_resolution_clock::now();
        m_directTime = std::chrono::duration<double, std::milli>(endDirect - startDirect).count();

        if (ret != 0)
        {
            FAIL() << "Direct run failed with code: " << ret;
        }

        std::cout << "\nTesting " << m_path << " for " << m_accessCount << " accesses.\n";
        std::cout << "Direct run time for " << m_path << ": " << m_directTime << " milliseconds\n";
    }
};

TEST_P(PerfTests, DebuggerPerfTests)
{
    std::vector<std::string> args{std::to_string(m_accessCount)};
    dbg::Variable var {"global_var"};
    dbg::Debugger debugger(m_path, args, var);

    std::vector<long> read;
    std::vector<long> write;

    // avoid any allocations during the test
    read.reserve(m_accessCount);
    write.reserve(m_accessCount);

    // clang-format off
    debugger.setOnRead(
        [&read](const dbg::Variable var)
        {
            read.push_back(var.get<long>());
        });

    debugger.setOnWrite(
        [&write](const dbg::Variable var)
        {
            write.push_back(var.get<long>());
        });
    // clang-format on

    // Measure debug runtime
    auto startDebug = std::chrono::high_resolution_clock::now();
    debugger.run();
    auto endDebug = std::chrono::high_resolution_clock::now();
    auto debugTime = std::chrono::duration<double, std::milli>(endDebug - startDebug).count();

    std::cout << "Debugger run time for " << m_path << ": " << debugTime << " milliseconds\n";
    std::cout << "Performance ratio (debugger / direct) for " << m_path << ": " << (debugTime / m_directTime) << "\n\n";

    ASSERT_EQ(read.size() + write.size(), m_accessCount);
}

// Define parameters {"path, accessCount"}
INSTANTIATE_TEST_SUITE_P(PerfTestsInstantiation, PerfTests,
    ::testing::Values(
        std::make_tuple("./raw", 10000),
        std::make_tuple("./real", 10000),
        std::make_tuple("./raw", 50000),
        std::make_tuple("./real", 50000)
    ));