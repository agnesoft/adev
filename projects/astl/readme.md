# astl

-   [Overview](#overview)
-   [Prerequisites](#prerequisites)
-   [Usage](#usage)
-   [Build](#build)
    -   [Clang](#clang)
    -   [GCC](#gcc)
    -   [MSVC](#msvc)
-   [libc++](#libc)

## Overview

The `astl` module is a convenience wrapper around all legacy C++ Standard Template Library (STL) headers. It effectively turns STL into a single module to be imported. By doing this it avoids the hassle of converting individual headers into header units. It also avoids clashes of multiply defined symbols that arise when using individual header units from STL.

## Prerequisites

-   C++ compiler
-   STL for the selected compiler

## Usage

```
import astl;

std::cout << "Hello, World!\n";
```

## Build

### Clang

```
clang++ -fmodules -Xclang -emit-module-interface astl.cpp
clang++ -fmodules -c astl.cpp
```

The header unit from `astl.hpp` is created automatically as part of compiling the module interface. The object file has to be compiled separately but contains everything. When used you might need `-fprebuilt-module-path` pointing to the directory of the `astl.pcm` module interface that is produced by the above commands. The object file must be linked in manually.

### GCC

```
g++ /std=c++20 -fmodules-ts -x c++-header astl.hpp
g++ /std=c++20 -fmodules-ts -c astl.cpp
```

The header unit must be compiled first, then the module interface. The object file is produced automatically. There is no separate object file for the header unit with GCC. When used the module interface should be picked up automatically as GCC uses `gcm.cache` for all compiled modules and header units and looks there for precompiled modules and header units. The object file must be linked in manually.

### MSVC

```
cl.exe /std:c++20 /exportHeader /c /TP astl.hpp
cl.exe /std:c++20 /interface /headerUnit "astl.hpp=astl.hpp.ifc" /c astl.cpp
```

The header unit must be compiled first and the command will also produce the object file for it. MSVC requires explicit mapping between headers and header units with `/headerUnit` on every invocation of the compiler. This requirement is transitive so you need the `/headerUnit` mapping when using the `astl` as well. When used you might also need `/ifcSearchDir` pointing to the directory with the produced `astl.ifc` module interface. Both object files - for the `astl.ifc` module interface and the `astl.hpp.ifc` header unit must be linked in manually.

## libc++

The `libc++` used by clang does not yet implement the `source_location` standard header (as of 03/11/2021, LLVM 13) and `astl` uses its own implementation.
