# ATest

C++ testing framework.

-   [Dependencies](#dependencies)
-   [Build](#build)
    -   [MSVC](#msvc)
    -   [clang](#clang)
-   [Known Issues](#known-issues)
    -   [Affects Users](#affects-users)
    -   [Does Not Affect Users](#does-not-affect-users)

## Dependencies

-   [ASTL](../astl/README.md)

## Build

### MSVC

```
cl /std:c++latest /headerUnit "project/astl/include/astl.hpp=astl.hpp.ifc" /c /Fo /interface atest.ixx
cl /std:c++latest /headerUnit "project/astl/include/astl.hpp=astl.hpp.ifc" test\atest_test.cpp atest.obj astl.obj
```

Everything must be compiled with the same flags that affect code generation including the `astl` dependency.

The `/interface` flag will generate the `ifc` (compiled module interface) for `atest`. Finally the `atest.ifc` does not need to be specified when compiling a translation unit that imports it if it can be found in the same directory. Header units mapping however still must be specified for each translation unit even transitively (like `astl.hpp=astl.hpp.ifc`). Note that both header units and the module interfaces must also produce regular object files (`/Fo`) so that the code can be linked together.

-   `/exportHeader` will compile a header into an `ifc` (a header unit).
-   `/interface` will compile a module into the `ifc`.
-   `/std:c++latest` enabled C++20 modules.
-   `/Fo` to produce an object file.
-   `/c` to perform compilation only.
-   `/TP` means that hte input files are all C++ source files.

### Clang

Using `clang++`:

```
mkdir -p build
cd build

clang++-11 -std=c++2a -I../astl/include -fmodules -Xclang -emit-module-interface -c -x c++ atest.ixx -o atest.pcm
clang++-11 -std=c++2a -I../astl/include -fmodules -fprebuilt-module-path=. -c -x c++ atest.ixx -o atest.o
clang++-11 -std=c++2a -I../astl/include -L<libc++pprefix>/lib -lc++ -fmodules -fprebuilt-module-path=. `-Wl,-rpath,<libcxx-install-prefix>/lib` test/atest_test.cpp atest.o -o atest_test

cd ..
```

Everything must be compiled with the same flags that affect code generation. The first command will compile a module interface into a precompiled module `pcm`. The second command will generate a regular object file from the module with the symbols used for linking purposes. The third command will compile the source file that imports the module and will also link to its object file.

-   `-std=c++2a` enables modules.
-   `-I../astl/include` adds `astl` dependency's include path that include its header and the `moduel.moduelmap` files.
-   `-fmodules` enables modules and implicit header unit creation and implicit module maps.
-   `-Xclang -emit-module-interface` compiles a module into `pmc`. Notice the flag needs to be forwarded to the compiler with `-Xclang`.
-   `-c` tells clang to only perform compilation and no linking.
-   `-x c++` tells clang to treat the following input file as C++ file. Required for non-standard extensions such as `*.ixx`.
-   `-fprebuilt-module-path=.` tells clang where to look for precompiled modules - int this case the current directory.
-   `-L<libc++pprefix>/lib` adds the `libc++` lib directory to the library search path for linking.
-   `-lc++` tells clang to link against `libc++`.
-   `-Wl,-rpath,<libcxx-install-prefix>/lib` embeds the `libc++` library path to the binary, so it can be found during runtime.

## Known Issues

### Affects Users

-   Using templated function such as `toThrow` (e.g. `expect([] { throw 1; }).toThrow<int>();` does not work and requires `template` keyword (e.g. `expect([] { throw 1; }).template toThrow<int>();`) otherwise parser fails: Reported bug: https://developercommunity.visualstudio.com/t/Cannot-instantiate-template-exported-fro/1387974

### Does Not Affect Users

-   [**All**] No STL vendor currently supports C++20 `source_location`. It is being emulated with `__builtin_FILE` that however is not `consteval` and does not really evaluate at the callsite. To force it to evaluate at the callsite the `source_location` and its callers `suite`. `test` and `expect` are all template functions so they are instantiated at the call site. Once actual `source_location` is available the custom version can be scrapped and `suite` and `test` may stop being template functions.
-   [**MSVC 16.9.3**] Using `type_info` requires `using ::type_info;` otherwise the `type_info` struct is unknown at compile time. Reported bug: https://developercommunity.visualstudio.com/t/Undefined-reference-to-type_info-when-us/1384072
-   [**libc++ 11.1.0**] C++20 `operator<<(std::chrono::duration)` is missing in `libc++` and requires a workaround in form of `duration.count() << "ms"` to emulate the operator. Should be removed once `libc++` implements this feature.
