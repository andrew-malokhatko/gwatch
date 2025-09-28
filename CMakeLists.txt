cmake_minimum_required(VERSION 3.28)
project(gwatch)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(dbg)

# Check system requirements
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(WARNING "This project targets Linux only. Current system: ${CMAKE_SYSTEM_NAME}. Build may fail.")
endif()

if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    message(WARNING "This project targets x86-64 only. Current processor: ${CMAKE_SYSTEM_PROCESSOR}. Build may fail.")
endif()

# build tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()

    find_package(GTest REQUIRED)
    if(NOT GTest_FOUND)
        message(FATAL_ERROR "Google Test not found. Please install it (e.g., 'sudo apt install libgtest-dev' on Ubuntu).")
    endif()

    add_subdirectory(tests)
endif()

add_executable(gwatch main.cpp)

# build examples
option(BUILD_EXAMPLES "Examples" ON)
if (BUILD_EXAMPLES)
    add_executable(cli_example tests/dummy/cli_example.cpp)
    target_compile_options(cli_example PRIVATE -g)

    add_dependencies(gwatch
            cli_example
    )
endif()

target_link_libraries(gwatch PRIVATE dbg)