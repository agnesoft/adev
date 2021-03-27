# ADev

Central development repository.

-   [Projects](#projects)
-   [Build](#build)
    -   [MSVC](#msvc)
    -   [clang](#clang)

## Projects

| Name                                | Path                               | Description                                              | Design                                      |
| ----------------------------------- | ---------------------------------- | -------------------------------------------------------- | ------------------------------------------- |
| [ABuild](projects/abuild/README.md) | [projects/abuild](projects/abuild) | C++ build system.                                        | [HLD](projects/abuild/high_level_design.md) |
| [ATest](projects/atest/README.md)   | [projects/atest](projects/atest)   | C++ testing framework.                                   | [HLD](projects/atest/high_level_design.md)  |
| [ASTL](projects/astl/README.md)     | [projects/astl](projects/astl)     | C++ Standard Template Library (STL) header unit wrapper. | [HLD](projects/astl/high_level_design.md)   |

## Build

### MSVC

Open Visual Studio Command Prompt and run `build.bat` in the root of the repository.

### clang

Run `build.sh` in the root of the repository.
