add_library(dbg
        include/Debugger.hpp
        src/Debugger.cpp
        include/Util.hpp
        src/Util.cpp
)

target_include_directories(dbg
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)