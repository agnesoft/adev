# ADev

## [BINARIES & ARTIFACTS](https://github.com/Agnesoft/ADev/actions?query=branch%3Amaster)

![Build](https://github.com/Agnesoft/ADev/workflows/ADev/badge.svg)

Central development repository that hosts all the Agnesoft projects providing common development & release (CI/CD) infrastructure. Use `./build.sh` script (see below) for setting things up, building, running checks, installing prerequisites & more.

## Projects

- [ABenchmarks](projects/ABenchmarks/README.md) Collection of benchmarks from other projects.
- [ABuild](projects/ABuild/README.md) C++ automatic build system.
- [ACore](projects/ACore/README.md) C++ basic containers and utilities.
- [ADb](projects/ADb/README.md) C++ graph database.
- [ADbStudio](projects/ADbStudio/README.md) Vue.js app to view & edit ADb database.
- [AFile](projects/AFile/README.md) C++ file based containers.
- [AGraph](proejcts/AFile/README.md) C++ graph implementation.

#### Third Party Dependencies

- [Catch2](https://github.com/catchorg/Catch2) C++ a multi-paradigm test framework.
- [tiny-process-library](https://gitlab.com/eidheim/tiny-process-library) C++ platform independent process library.

## Build

To build **C++** run (use Git Bash on _Windows_):

```
./build.sh build
```

To build **Vue.js** run (use Git Bash on _Windows_):

```
./build.sh build-vuejs
```

To print all avilable actions run (use Git Bash on _Windows_):

```
./build.sh
```

#### Prerequisites

All required tools except the OS package managers can be installed with `./build.sh instal-xxx`. Run `./build.sh` without parameters to see all the available options or refer to the table below:

| Prerequisite                                                         | Version | OS      | Language | `./build.sh`           | Required For (`./build.sh`)                                                                            |
| -------------------------------------------------------------------- | ------- | ------- | -------- | ---------------------- | ------------------------------------------------------------------------------------------------------ |
| [Chocolatey](https://chocolatey.org/)                                | any     | Windows | -        | -                      | `install-xxx`                                                                                          |
| [apt](<https://en.wikipedia.org/wiki/APT_(software)>)                | any     | Linux   | -        | -                      | `install-xxx`                                                                                          |
| [Homebrew](https://brew.sh/)                                         | any     | macOS   | -        | -                      | `install-xxx`                                                                                          |
| [CMake](https://cmake.org/)                                          | 3.15+   | Any     | C++      | `install-cmake`        | `build`                                                                                                |
| [Ninja](https://ninja-build.org/)                                    | 1.10+   | Any     | C++      | `install-ninja`        | `build`                                                                                                |
| [GCC](https://gcc.gnu.org/)                                          | 9+      | Linux   | C++      | `install-gcc`          | `build` & `tests` & `benchmarks`                                                                       |
| [MSVC](https://visualstudio.microsoft.com/cs/vs/features/cplusplus/) | 14+     | Windows | C++      | `install-msvc`         | `build` & `tests` & `benchmarks`                                                                       |
| [Clang](https://clang.llvm.org/)                                     | 9+      | Any     | C++      | `install-clang`        | `build` & `tests` & `benchmarks` & `sanitize-address` & `sanitize-memory` & `sanitize-ub` & `coverage` |
| [Clang-Format](https://clang.llvm.org/docs/ClangFormat.html)         | 9+      | Any     | C++      | `install-clang-format` | `formatting`                                                                                           |
| [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/)               | 9+      | Any     | C++      | `install-clang-tidy`   | `analysis`                                                                                             |
| [Doxygen](https://www.doxygen.nl/index.html)                         | 1.8+    | Any     | C++      | `install-doxygen`      | `documentation`                                                                                        |
| [LLVM](https://www.llvm.org)                                         | 9+      | Any     | C++      | `install-llvm`         | `coverage`                                                                                             |
| [Node.js](https://nodejs.org/)                                       | 12+     | Any     | JS       | `install-vuejs`        | `build-vuejs` & `tests-vuejs`                                                                          |
| [Vue.js](https://vuejs.org/)                                         | 2.2     | Any     | JS       | `install-vuejs`        | `build-vuejs` & `tests-vuejs`                                                                          |

## Continuous Integration (GitHub Actions)

All of the following checks are part of the `build.sh` script and can be run locally on your system. Run `./build.sh` without parameters for details. The following table describes the status checks that are run with every pull request and master build:

| Check                                                                                      | Requirement                                                               | Description                                                                                                                                                                                                       | OS      | Language | `build.sh`                               |
| ------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------- | -------- | ---------------------------------------- |
| Build Windows (**[MSVC](https://visualstudio.microsoft.com/cs/vs/features/cplusplus/)**)   | Compiles <br/> No warnings <br/> 100x Tests Pass <br/> 1x Benchmarks Pass | Windows MSVC x64 Release build. Runs tests 100 times to prevent flaky tests. Runs benchmarks once.                                                                                                                | Windows | C++      | `build` <br/> `tests` <br/> `benchmarks` |
| Build Linux (**[GCC](https://gcc.gnu.org/)**)                                              | Compiles <br/> No warnings <br/> 100x Tests Pass <br/> 1x Benchmarks Pass | Linux GCC x64 Release build. Runs tests 100 times to prevent flaky tests. Runs benchmarks once.                                                                                                                   | Linux   | C++      | `build` <br/> `tests` <br/> `benchmarks` |
| Build macOS (**[Clang](https://clang.llvm.org/)**)                                         | Compiles <br/> No warnings <br/> 100x Tests Pass <br/> 1x Benchmarks Pass | macOS Clang x64 Release build. Runs tests 100 times to prevent flaky tests. Runs benchmarks once.                                                                                                                 | macOS   | C++      | `build` <br/> `tests` <br/> `benchmarks` |
| Build Vue.js (**[Vue.js](https://vuejs.org/)**)                                            | Compiles <br/> Tests pass                                                 | Builds Vue.js applications & runs the tests.                                                                                                                                                                      | Any     | JS       | `build-vuejs` <br/> `tests-vuejs`        |
| [Analysis](https://clang.llvm.org/extra/clang-tidy/)                                       | No warnings                                                               | Clang-Tidy static analysis. Runs on all the C++ sources except the third party code. Suppress undesirable warnings and diagnostics with `NOLINT`.                                                                 | Any     | C++      | `analysis`                               |
| [Coverage](https://clang.llvm.org/docs/SourceBasedCodeCoverage.html)                       | 100 % line <br/> 100% function _any%_ region                              | LLVM code coverage. Region (branch) coverage is _any%_ due to the tooling's inability to handle `constexpr`.                                                                                                      | Any     | C++      | `coverage`                               |
| [Documentation](https://www.doxygen.nl/index.html)                                         | Full documentation                                                        | Doxygen code documentation. All public/protected entities must be documented unless they are explicitly part of the implementation detail (guard these with `//! \cond IMPLEMENTAION_DETAIL` and `//! \endcond`). | Any     | C++      | `documentation`                          |
| [Formatting](https://clang.llvm.org/docs/ClangFormat.html)                                 | Everything formatted                                                      | Clang-Format formatting tool. Runs on all the C++ sources except the third party code. Use `// clang-format off` and `// clang-format on` to selectively suppress the automatic formatting.                       | Any     | C++      | `formatting`                             |
| [Sanitize Address](https://clang.llvm.org/docs/AddressSanitizer.html)                      | No bug                                                                    | Clang's Address Sanitizer. Builds with Clang & runs tests.                                                                                                                                                        | Linux   | C++      | `sanitize-address`                       | [ASAN] |
| [Sanitize Memory](https://clang.llvm.org/docs/MemorySanitizer.html)                        | No bug                                                                    | Clang's Memory Sanitizer. Builds with Clang & runs tests. Downloads and builds LLVM/Clang `libc++` that is required for this sanitizer.                                                                           | Linux   | C++      | `sanitize-memory`                        |
| [Sanitize Undefined Behavior](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) | No bug                                                                    | Clang's Undefined Behavior Sanitizer. Builds with Clang & runs tests.                                                                                                                                             | Linux   | C++      | `sanitize-ub`                            |

## Development

#### C++

Open `CMakeLists.txt` in your favorite IDE.

#### Vue.js

Choose from [Vue.js Dev Tools](https://vuejs.org/v2/guide/installation.html).
