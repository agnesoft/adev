# ATest

C++ testing framework.

## Build

Compiler toolchain specific instructions:

### MSVC

Using `MSBuild`:

-   Set `Configuration Properties -> C/C++ -> Language -> C++ Language Standard` in your project to `Preview - Features from the Latest C++ Working Draft (/std:c++latest)`.
-   Add `atest.ixx` to your project and import it in a cpp file.

Using `cl.exe`:

-   Open Visual Studio command prompt and run:

```
mkdir build
cd build

set CPP_FLAGS=/nologo /std:c++latest /EHsc

cl %CPP_FLAGS% /exportHeader "%VCToolsInstallDir%include\concepts"
cl %CPP_FLAGS% /exportHeader "%VCToolsInstallDir%include\iostream"
cl %CPP_FLAGS% /exportHeader "%VCToolsInstallDir%include\sstream"

set CPP_FLAGS_ATEST=%CPP_FLAGS% /headerUnit "%VCToolsInstallDir%include\concepts=concepts.ifc" /headerUnit "%VCToolsInstallDir%include\iostream=iostream.ifc" /headerUnit "%VCToolsInstallDir%include\sstream=sstream.ifc"

cl %CPP_FLAGS_ATEST% /interface -c ..\atest.ixx
cl %CPP_FLAGS_ATEST% ..\test\atest_test.cpp

cd ..
```

The first three commands will create `header units` from the three STL headers used by `atest`. The headers then need to be mapped to the created `ifc` files when creating `atest.ifc` and also when importing it in other files. The `/interface` flag will generate the `ifc` for `atest`. Finally the `atest.ifc` does not need to be specified when compiling a translation unit that imports it but the header units used in must!

Few things to note:

-   Everything must be compiled using the very same flags when using C++20 modules. Additional flags can be used such as optimization, warnings etc. but they must be consistent across all compilations and exports of the `header units`.
-   EHsc is required for C++ exceptions used by `atest`.
-   Header units mappings to STL headers must be specified for each translation unit that imports `atest`. Hopefully this will not bee needed once MSVC ships with STL as modules.

### Clang

Using `clang++`:

```
mkdir build
cd build

CPP_FLAGS=-std=c++2a

clang++ $CPP_FLAGS -Xclang -emit-module-interface -c -x c++ ../atest.ixx -o atest.pcm
clang++ $CPP_FLAGS -fprebuilt-module-path=. -fimplicit-modules -fimplicit-module-maps ../test/atest_test.cpp

cd ..
```

Everything must be compiled with the same flags so use a variable for them. The `-Xclang -emit-module-interface` tells clang compiler to create a precompiled module (interface). The `-c` lets it run only compilation and no linking. The `-x c++` tells it to treat the input file as a C++ source file which is required for non-standard (but kind of useful `*ixx` extension). The output specified with `-o` is set to `*.pcm` which is what Clang expects to find when searching for imported module interfaces.

The second command specifies `-fprebuilt-module-path=.` which is the path it will search for imported modules (i.e. when in the source there is `import atest;` it will search this path for `atest.pcm`). The `-fimplicit-module-maps` tells the compiler to implicitly map standard includes into imports of header units.

### GCC

TODO
