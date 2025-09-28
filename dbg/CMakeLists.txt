add_library(dbg
        include/Debugger.hpp
        include/Variable.hpp
        src/Debugger.cpp
        src/Util.cpp
        src/Util.hpp
        src/Variable.cpp
)

target_include_directories(dbg
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)