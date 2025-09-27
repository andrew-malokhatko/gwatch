cmake_minimum_required(VERSION 4.0)
project(gwatch)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(dbg)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(WARNING "This project targets Linux only. Current system: ${CMAKE_SYSTEM_NAME}. Build may fail.")
endif()

if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    message(WARNING "This project targets x86-64 only. Current processor: ${CMAKE_SYSTEM_PROCESSOR}. Build may fail.")
endif()

#option(BUILD_TESTING "Build tests" ON)
#if(BUILD_TESTING)
    enable_testing()

    find_package(GTest REQUIRED)
    if(NOT GTest_FOUND)
        message(FATAL_ERROR "Google Test not found. Please install it (e.g., 'sudo apt install libgtest-dev' on Ubuntu).")
    endif()

    add_subdirectory(tests)

#endif()

add_executable(gwatch main.cpp)

target_link_libraries(gwatch PRIVATE dbg)