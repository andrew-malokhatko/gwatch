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
> but I could not find a way to reliable demangle them without external
> tools. As a result, C++ globals are not fully supported in this implementation.

### Global varialbes
This program track only non-static C global variables, which are
listed in the symbol table without any compiler-applied mangling.
Singed types are possible, but are interpreted as unsigned, as
sign can't be inferred without parsing DWARF parsing. To address this
a separate option `--svar` as an alternative for `--var` for signed variables.
Finally, reads from `const` global variables are not tracked as compilers
inline their value, preventing memory access, the only exception is `volatile const`
global variables.


### Compilers
Use g++.

### Testing
Performance tests and Functional tests 