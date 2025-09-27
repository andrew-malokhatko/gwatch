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

# Build dummy programs
add_executable(hello_world dummy/hello_world.cpp)
add_executable(arg_printer dummy/arg_printer.cpp)
add_executable(factorial dummy/factorial.cpp)
add_executable(one_read dummy/one_read.cpp)
add_executable(one_read_short dummy/one_read_short.cpp)
add_executable(one_read_long dummy/one_read_long.cpp)
add_executable(one_write dummy/one_write.cpp)
add_executable(raw dummy/raw.cpp)
add_executable(real dummy/real.cpp)

# Add debug info (-g) to each dummy
target_compile_options(hello_world PRIVATE -g)
target_compile_options(arg_printer PRIVATE -g)
target_compile_options(factorial PRIVATE -g)
target_compile_options(one_read PRIVATE -g)
target_compile_options(one_read_short PRIVATE -g)
target_compile_options(one_read_long PRIVATE -g)
target_compile_options(one_write PRIVATE -g)
target_compile_options(raw PRIVATE -g)
target_compile_options(real PRIVATE -g)

add_dependencies(debugger_tests
        hello_world
        arg_printer
        factorial
        one_read
        one_read_short
        one_read_long
        one_write
)

add_dependencies(perf_tests
        raw
        real
)