# ASTL

C++ Standard Template Library (STL) header unit wrapper.

-   [Generator](#generator)
-   [Build](#build)
    -   [MSVC](#msvc)
    -   [clang](#clang)

## Generator

The generator is written in Node.js and will output or update the `include/astl.hpp` when run:

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
-   Link againsst the astl.obj as well.

## clang
