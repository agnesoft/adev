# ADev

Central development repository.

-   [Projects](#projects)
-   [Build](#build)
    -   [MSVC](#msvc)
    -   [clang](#clang)
-   [Tools](#tools)
    -   [clang-format](#clang-format)
    -   [clang-tidy](#clang-tidy)

## Projects

| Name                                      | Path                                     | Description                                              | Links                                       |
| ----------------------------------------- | ---------------------------------------- | -------------------------------------------------------- | ------------------------------------------- |
| [ACore](projects/acore/README.md)         | [projects/acore](projects/acore)         | C++ library that provides generic functionality.         | [HLD](projects/acore/high_level_design.md)  |
| [ABuild](projects/abuild/README.md)       | [projects/abuild](projects/abuild)       | C++ build system.                                        | [HLD](projects/abuild/high_level_design.md) |
| [ATest](projects/atest/README.md)         | [projects/atest](projects/atest)         | C++ testing framework.                                   | [HLD](projects/atest/high_level_design.md)  |
| [ASTL](projects/astl/README.md)           | [projects/astl](projects/astl)           | C++ Standard Template Library (STL) header unit wrapper. | [HLD](projects/astl/high_level_design.md)   |
| [RapidJSON](projects/rapidjson/README.md) | [projects/rapidjson](projects/rapidjson) | C++ JSON parser and generator.                           | [License](projects/rapidjson/license)       |

## Build

### MSVC

Open Visual Studio Command Prompt and run `build.bat` in the root of the repository.

### clang

Run `build.sh` in the root of the repository. You may need to set the two variables at the top of the script to reflect your system:

-   `CLANG` is the clang executable to use, e.g. `clang++-11`
-   `LIBCXX_PREFIX` is the location of your `libc++` installation/build that contains include and lib directories.

## Tools

### clang-format

The `.clang-format` configuration file is present int the root of the repository and should be used for formatting of all C++ source files.

### clang-tidy

**Currently only clang with libc++ are supported**

The `.clang-tidy` configuration file is present in the root of the repository along with the `compile-flags.txt` that are used by the `clang-tidy` tool to perform the static analysis on the source files. The analysis of all files can be performed with `./clang-tidy.sh`.
