# ATest

C++ testing framework.

## Build

Compiler toolchain specific instructions:

### MSVC

Using `MSBuild`:

-   Set `Configuration Properties -> C/C++ -> Language -> C++ Language Standard` in your project to `Preview - Features from the Latest C++ Working Draft (/std:c++latest)`.
-   Add `atest.ixx` to your project and import it in a cpp file.

Using `cl.exe`:

```
cl /std:c++latest /EHsc /exportHeader <path to STL>\concepts
cl /std:c++latest /EHsc /exportHeader <path to STL>\iostream
cl /std:c++latest /EHsc /exportHeader <path to STL>\sstream
cl /std:c++latest /EHsc /headerUnit "<path to STL>\concepts=concepts.ifc" "<path to STL>\iostream=iostream.ifc" "<path to STL>\concepts=sstream.ifc" /interface -c atest.ixx
cl /std:c++latest /EHsc /headerUnit "<path to STL>\concepts=concepts.ifc" "<path to STL>\iostream=iostream.ifc" "<path to STL>\concepts=sstream.ifc" your_test.cpp
```

The first three commands will create `header units` from the three STL headers used by `atest`. The headers then needs to be mapped to the created `ifc` files when created `atest.ifc` itself. The `/interface` flag will generate the `ifc` for `atest`. Finally the `atest.ifc` does not need to be specified when compiling a translation unit that imports it but the header units used in still must!

Few things to note:

-   Everything must be compiled using the very same flags when using C++20 modules. Additional flags can be used such as optimization, warnings etc. but they must be consistent across all compilations of the `header units`, `atest` and your test source files.
-   EHsc is required for C++ exceptions used by `atest`.
-   Header units mappings to STL headers must be specified for each translation unit that imports `atest`. Hopefully this will not bee needed once MSVC ships with STL as modules.

### Clang

TODO

### GCC

TODO
