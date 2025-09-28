add_executable(debugger_tests
        DebuggerTests.cpp
)

add_executable(perf_tests
        PerfTests.cpp
)

target_link_libraries(debugger_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        dbg
)

target_link_libraries(perf_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        dbg
)

add_test(NAME DebuggerTests COMMAND debugger_tests)
add_test(NAME PerfTests COMMAND perf_tests)

# Build dummy programs
add_executable(one_read dummy/one_read.cpp)
add_executable(one_read_short dummy/one_read_short.cpp)
add_executable(one_read_long dummy/one_read_long.cpp)
add_executable(one_write dummy/one_write.cpp)
add_executable(thread_read dummy/thread_read.cpp)
add_executable(thread_write dummy/thread_write.cpp)
add_executable(thread_multi dummy/thread_multi.cpp)
add_executable(thread_indirect dummy/thread_indirect.cpp)

add_executable(raw dummy/raw.cpp)
add_executable(real dummy/real.cpp)

# Add debug info (-g) to each dummy
target_compile_options(one_read PRIVATE -g)
target_compile_options(one_read_short PRIVATE -g)
target_compile_options(one_read_long PRIVATE -g)
target_compile_options(one_write PRIVATE -g)
target_compile_options(thread_read PRIVATE -g)
target_compile_options(thread_write PRIVATE -g)
target_compile_options(thread_multi PRIVATE -g)
target_compile_options(thread_indirect PRIVATE -g)

target_compile_options(raw PRIVATE -g)
target_compile_options(real PRIVATE -g)

add_dependencies(debugger_tests
        one_read
        one_read_short
        one_read_long
        one_write
        thread_read
        thread_write
        thread_multi
        thread_indirect
)

add_dependencies(perf_tests
        raw
        real
)