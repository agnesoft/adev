# astl

## Overview

The `astl` module is the convenience wrapper around all legacy C++ Standard Template Library (STL) headers. It effectively turns STL into a single module to be imported. By doing this it avoids the hassle of converting individual headers into header units. It also avoids clashes of multiply defined symbols that arise when using individual header units from STL.

## Prerequisites

-   C++ compiler
-   STL for the selected compiler

## Build

```
//Clang
clang++ -fmodules -Xclang -emit-module-interface astl.cpp #builds the interface and implicitly creates header unit from astl.hpp
clang++ -fmodules -c astl.cpp #builds the object file

//GCC
g++ /std=c++20 -fmodules-ts -x c++-header astl.hpp #builds the header unit from astl.hpp
g++ /std=c++20 -fmodules-ts -c astl.cpp #builds the precompiled module & object file

//MSVC
cl.exe /std:c++20 /exportHeader /c /TP astl.hpp #builds the header unit from astl.hpp
cl.exe /std:c++20 /interface /headerUnit "astl.hpp=astl.hpp.ifc" /c astl.cpp #builds the precompiled module & object file
```

Each compiler will produce an object file and a precompiled module file with the above steps. The precompiled module file should be found by the compiler automatically or via its standard mechanism it uses:

-   `-fprebuilt-module-path` for clang
-   `/ifcSearchDir` for MSVC
-   GCC uses `/gcm.cache/` by default where it stores and finds precompiled interfaces, nothing to be specified by the user

The object file needs to be linked manually to your executable just like any other library when the module is imported.

NOTE: MSVC requires the header unit to be specified transitively, i.e. it does not get embedded into the `astl` module. Therefore you need `/headerUnit` option mapping the `astl.hpp` to its `ifc` file for every usage of `astl` with MSVC.

## Usage

```
import astl;

std::cout << "Hello, World!\n";
```

## std::source_location

The `libc++` used by clang does not yet implement the `source_location` standard header. If that header is not available `astl` will use its own implementation.
