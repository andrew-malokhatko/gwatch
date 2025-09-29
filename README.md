# Overview
`gwatch` is a lightweight debugger designed to monitor
and report changes to global variables in C programs.
It leverages Linux `ptrace` along with hardware debug registers (`DR0–DR7`)
to efficiently track memory accesses at runtime.

# Assumptions & Limitations 

### Architecture
This program is designed to run only on Linux x64.
It relies on hardware debug registers (`DR0` - `DR7`), so behavior
on non-Intel/AMD CPU's is not guaranteed.

### Processes
This solution launches process from an `ELF` executable.
For this task, I also assume that `ELF` files are compiled in C with
debug information included (e.g, -g option in gcc).

The solution has partial support for C++ global variables, but it is unreliable
due to name mangling. It works correctly only if the global variable:
- Is outside namespaces and classes, and
- does not collide with other symbols in symbol table.

> [NOTE]
> I initially planned to support C++ global variables as well,
> but I could not find a way to reliably demangle them without external
> tools. As a result, C++ globals are not fully supported in this implementation.

### Global variables
This program tracks only non-static C global variables, which are
listed in the symbol table without any compiler-applied mangling.
Singed types are possible, but are interpreted as unsigned, as
sign can't be inferred without DWARF parsing. To address this,
a separate option `--svar` is present as an alternative for `--var` for signed variables.
Finally, reads from `const` global variables are not tracked as modern compilers
inline their value, preventing memory access, the only exception is `volatile const`
global variables.


### Compilers
Use g++. I can't guarantee behavior for any other compiler since all
of my tests and examples hardly rely on g++ name mangling, which is not a
part of C++ standard.

# Dependencies / Requirements

To build and run `gwatch`, you will need:

- **Operating System**: Linux x86_64
- **CPU**: Intel/AMD with hardware debug registers (`DR0–DR7`)
- **Compiler**: g++ version ≥ 13.3
- **CMake**: version ≥ 3.28
- **GoogleTest (GTest)**: for running the unit and performance tests

# Build & Run
The easiest way to build and test this program is to run the `autotest.sh` script.
It will automatically build the project and run a set of simple tests.

### Manual Build

If you prefer to build manually, run:

```shell
mkdir build
cd build
cmake ..
cmake --build .
```

### Build Options
This project provides two optional build flags
- `BUILD_TEST` - Build unit and performance tests (default: ON)
- `BUILD_EXAMPLES` - Build example programs (default: ON)

You can disable them during configuration: 
```shell
cmake .. -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF 
```

# Usage

Run the debugger with:
```shell
./gwatch (--var | --svar) <symbol> --exec <path> [-- arg1 ... argN]
```

- --var <symbol>: Track an unsigned global variable.
- --svar <symbol>: Track a signed global variable.
- --exec <path>: Path to the program you want to debug.
- [-- arg1 ... argN]: Optional arguments passed to the debugged program.

# Examples

A sample executable `cli_example` is provided, which updates three global variables  
`a`, `b`, and `c` once every second. You can try it out as follows:

```shell
./gwatch --var a --exec ./cli_example
```

or

```shell
./gwatch --svar b --exec ./cli_example
```

# Tests
This project is tested using both **GoogleTest (GTest)** and the `autotest.sh` script.

### GTest

There are two collections of tests implemented with GTest:
- **DebuggerTests** (functional tests)
- **PerfTests** (performance tests)

To run all tests:
```shell
cd build
ctest
```

To run specific test, pass -R with the test name:
```shell
ctest -R <test_name>
```
To see detailed output of performance tests, run with --verbose:
```shell
ctest -R PerfTests --verbose 
```

You can find all the test programs in `tests/dummy` directory.

### autotest.sh

The `autotest.sh` script builds the project and runs a predefined test program.
Run it from the project root:
```shell
./autotest [-b build_dir] [-d debugger] [-t test_prog] [-s symbol]
```
- `-b build_dir` : Custom build directory (default: build)
- `-d debugger` : Path to the debugger executable (default: build/gwatch)
- `-t test_prog` : Test program to run (default: tests/real)
- `-s symbol` : Watched global variable (default: global_var) 

All arguments are optional.
