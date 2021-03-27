# ASTL

C++ Standard Template Library (STL) header unit wrapper.

-   [Build](#build)
    -   [MSVC](#msvc)
        -   [MSBuild](#msbuild)
        -   [cl.exe](#clexe)
            -   [Usage](#usage)
    -   [clang](#clang)

## Build

Compiler toolchain specific instructions:

### MSVC

#### MSBuild

-   Set `Configuration Properties -> C/C++ -> Language -> C++ Language Standard` in your project to `Preview - Features from the Latest C++ Working Draft (/std:c++latest)`.
-   Add `astl.hpp` to your project.
-   Import it as `import "astl.hpp";` and MSBuild will produce the STL header unit automagically.

#### cl.exe

```
cl.exe /c /exportHeader /Fo /TP astl.hpp
```

-   `/c` stop before linking.
-   `/exportHeader` compiles the header into a header unit (ifc) named `astl.hpp.ifc` to be importable by users.
-   `/Fo` compiles the header into an object file (obj) for linking.
-   `/TP` considers the input file to be a translation unit.

##### Usage

Import it in code as:

```
import "astl.hpp";
```

Build command:

```
cl.exe /headerUnit "<ASTL path>\astl.hpp=<ASTL IFC path>\astl.hpp.ifc" main.cpp <ASTL obj Path>\astl.obj
```

-   `/headerUnit` maps the header file being imported to the `ifc`.

## clang
