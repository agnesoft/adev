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

cl /nologo /std:c++latest /EHsc /exportHeader "%VCToolsInstallDir%include\iostream"
cl /nologo /std:c++latest /EHsc /headerUnit "%VCToolsInstallDir%include\iostream=iostream.ifc" /interface -c ..\atest.ixx
cl /nologo /std:c++latest /EHsc /headerUnit "%VCToolsInstallDir%include\iostream=iostream.ifc" ..\test\atest_test.cpp

cd ..
```

Everything must be compiled with the same flags that affect code generation.

The first command will create a `header unit` from an STL header used by `atest`. The header then needs to be mapped to the created `ifc` file when creating `atest.ifc` and also when importing it in other files. The `/interface` flag will generate the `ifc` for `atest` itself in the second command. Finally the `atest.ifc` does not need to be specified when compiling a translation unit that imports it if it can be found in the same directory. Header units mapping however still must be specified for each translation unit even transitively. This will likely be resolved by Microsoft soon (written in 3/2021).

-   `/EHsc` is required for C++ exceptions.
-   `/exportHeader` will compile a header into an `ifc` (a header unit).
-   `/interface` will compile a module into the `ifc`.
-   `/headerUnit` is used for mapping exported headers in `ifc` form to the original header files and is required for every compilation that uses it even indirectly (transitively).
-   `/nologo` suppresses compiler banners in the output.
-   `/std:c++latest` enabled C++20 modules.

### Clang

Using `clang++`:

```
mkdir -p build
cd build

clang++-11 -std=c++2a -nostdinc++ -I<libc++pprefix>/include/c++/v1 -fimplicit-modules -fimplicit-module-maps -Xclang -emit-module-interface -c -x c++ ../atest.ixx -o atest.pcm
clang++-11 -std=c++2a -nostdinc++ -I<libc++pprefix>/include/c++/v1 -fprebuilt-module-path=. -fimplicit-modules -fimplicit-module-maps -c -x c++ ../atest.ixx -o atest.o
clang++-11 -std=c++2a -nostdinc++ -I<libc++pprefix>/include/c++/v1 -L<libc++pprefix>/lib -lc++ -fprebuilt-module-path=. -fimplicit-modules -fimplicit-module-maps ../test/atest_test.cpp atest.o

cd ..
```

Everything must be compiled with the same flags that affect code generation and you will need to use `libc++` ([Using libc++](https://libcxx.llvm.org/docs/UsingLibcxx.html)) instead of `stdlibc++`. The first command will compile a module interface into a precompiled module `pcm`. The second command will generate a regular object file from the module with the symbols used for linking purposes. The third command will compile the source file that imports the module and will also link to its object file. The header units from STL are created automatically thanks to `-fimplicit-modules` and `-fimplicit-module-maps`. Note that regular `stdlibc++` headers cannot be used for creating `header units`, you will get weird errors. You must use `libc++` that is modules aware. This will likely be resolved once C++20 modules are merged into the mainline GCC release soon (written in 3/2021).

-   `-std=c++2a` enables modules.
-   `nostdinc++` prevents the use of standard system headers (the `stdlibc++`).
-   `-I<libc++pprefix>/include/c++/v1` tells the compiler where to get STL headers (`libc++`).
-   `-fimplicit-modules` will look for precompield modules (`pmc`) based on the import name (e.g. `import atest` -> `atest.pmc`).
-   `-fimplicit-module-maps` will create header units from imported standard headers (e.g. `import <iostream>;`).
-   `-Xclang -emit-module-interface` compiles a module into `pmc`. Notice the flag needs to be forwarded to the compiler with `-Xclang`.
-   `-c` tells clang to only perform compilation and no linking.
-   `-x c++` tells clang to treat the following input file as C++ file. Required for non-standard extensions such as `*.ixx`.
-   `-fprebuilt-module-path=.` tells clang where to look for precompiled modules - int this case the current directory.
-   `-L<libc++pprefix>/lib` adds the `libc++` lib directory to the library search path for linking.
-   `-lc++` tells clang to link against `libc++`.

Note that if your `libc++` is not installed in the system and you linked to it dynamically you will then need `export LD_LIBRARY_PATH=<libc++pprefix>/lib` before you can run the application.
