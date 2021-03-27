# ASTL

C++ Standard Template Library (STL) header unit wrapper.

-   [Generator](#generator)
-   [Build](#build)
    -   [MSVC](#msvc)
    -   [clang](#clang)

## Generator

The generator is written in Node.js and will output or update the `include/astl.hpp` and `include/module.modulemap` (for clang) when run:

```
cd generator
npm install
npm start
```

## Build

Compiler toolchain specific instructions:

### MSVC

```
cl.exe /c /std:c++latest /exportHeader /Fo /TP astl.hpp
```

-   `/c` stop before linking.
-   `/std:c++latest` enabled modules.
-   `/exportHeader` compiles the header into a header unit (ifc) named `astl.hpp.ifc` to be importable by users.
-   `/Fo` compiles the header into an object file (obj) for linking.
-   `/TP` considers the input file to be a translation unit.

Import it in code as:

```
import "astl.hpp";
```

Build command of the user:

```
cl.exe /std:c++latest /headerUnit "<ASTL path>\astl.hpp=<ASTL IFC path>\astl.hpp.ifc" main.cpp <ASTL obj Path>\astl.obj
```

-   `/std:c++latest` enables modules.
-   `/headerUnit` maps the header file being imported to the `ifc` in format (`"<path to hpp>=<path to its ifc>"`).
-   Link against the astl.obj as well.

### clang

```
clang++ -std=c++20 -nostdinc++ -fmodules -Iinclude/ -I<libcxx-install-prefix>/include/c++/v1 -L<libcxx-install-prefix>/lib -lc++ -Wl,-rpath,<libcxx-install-prefix>/lib test/main.cpp -o test/test.out
```

-   Requires `clang` 11 or later.
-   Requires `libc++` (could possibly work with `stdlibc++` from GCC 10+, does not work with GCC9).
-   `-std=c++20` language standard.
-   `-nostdinc++` disables system include paths.
-   `-fmodules` enables modules including implict modules, implicit header units and implicit module maps.
-   `-Iinclude/` path to the `astl.hpp`.
-   `-I<libcxx-install-prefix>/include/c++/v1` path to the `libc++` headers.
-   `-L<libcxx-install-prefix>/lib` path to the `libc++` libraries.
-   `-lc++` links against `libc++`.
-   `-Wl,-rpath,<libcxx-install-prefix>/lib` embeds the `libc++` library path to the binary, so it can be found during runtime.

Note that `clang` is using `module.modulemap` files to map headers to header units. When implicit detection & generation is enabled (implied by `-fmodules`) it will pick these files when located in current directory or beside the header, generate the header unit with the current compiler options and map it to the compilation of the translation unit(s) and adds the object file output to the linker.
