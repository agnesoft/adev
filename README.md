# ADev

## [BINARIES & ARTIFACTS](https://github.com/Agnesoft/ADev/actions?query=branch%3Amaster)

![Build](https://github.com/Agnesoft/ADev/workflows/ADev/badge.svg)

Central development repository that host all Agnesoft projects providing common development & release (CI/CD) infrastructure. Use `./build.sh` script (see below) for setting things up, building, running checks, installing prerequisites & more.

## Projects

- [ABenchmarks](projects/ABenchmarks/README.md) Collection of benchmarks from other projects.
- [ABuild](projects/ABuild/README.md) C++ automatic build system.
- [ACore](projects/ACore/README.md) C++ basic containers and utilities.
- [ADb](projects/ADb/README.md) C++ graph database.
- [ADbStudio](projects/ADbStudio/README.md) Vue.js app to view & edit ADb database.
- [AFile](projects/AFile/README.md) C++ file based containers.
- [AGraph](proejcts/AFile/README.md) C++ graph implementation.

#### Third Party Projects

- [Catch2](https://github.com/catchorg/Catch2) C++ a multi-paradigm test framework.
- [tiny-process-library](https://gitlab.com/eidheim/tiny-process-library) C++ platform independent process library.

## Build

To build C++ run (use Git Bash on **Windows**):

```
./build.sh build
```

To build Vue.js run (use Git Bash on **Windows**):

```
./build.sh build-vuejs
```

To print other avilable actions run `./build.sh` without parameters.

#### Prerequisites

All required tools except for _C++ compilers_ themselves can be installed with `./build.sh instal-xxx`. Run `./build.sh` without parameters to see all the available options.

| Prerequisite                                                         | Version | ./build.sh           | Required For (`./build.sh`)                                                                                |
| -------------------------------------------------------------------- | ------- | -------------------- | ---------------------------------------------------------------------------------------------------------- |
| [CMake](https://cmake.org/)                                          | 3.15+   | install-cmake        | C++ `build`                                                                                                |
| [Ninja](https://ninja-build.org/)                                    | 1.10+   | install-ninja        | C++ `build`                                                                                                |
| [MSVC](https://visualstudio.microsoft.com/cs/vs/features/cplusplus/) | 14+     | install-msvc         | Only on Windows. C++ `build` & `tests` & `benchmarks`                                                      |
| [GCC](https://gcc.gnu.org/)                                          | 9+      | install-gcc          | Only on Linux. C++ `build` & `tests` & `benchmarks`                                                        |
| [Clang](https://clang.llvm.org/)                                     | 9+      | install-clang        | C++ `build` & `tests` & `benchmarks` & `sanitize-address` & `sanitize-memory` & `sanitize-ub` & `coverage` |
| [Node.js](https://nodejs.org/)                                       | 12+     | install-vuejs        | JS `build-vuejs` & `tests-vuejs`                                                                           |
| [Vue.js](https://vuejs.org/)                                         | 2.2     | install-vuejs        | JS `build-vuejs` & `tests-vuejs`                                                                           |
| [Doxygen](https://www.doxygen.nl/index.html)                         | 1.8+    | install-doxygen      | C++ `documentation`                                                                                        |
| [LLVM](https://www.llvm.org)                                         | 9+      | install-llvm         | C++ `coverage`                                                                                             |
| [Clang-Format](https://clang.llvm.org/docs/ClangFormat.html)         | 9+      | install-clang-format | C++ `formatting`                                                                                           |
| [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/)               | 9+      | install-clang-tidy   | C++ `analysis`                                                                                             |
| [Chocolatey](https://chocolatey.org/)                                | any     | -                    | Only on Windows. Required for `install-xxx` to work.                                                       |
| [apt](<https://en.wikipedia.org/wiki/APT_(software)>)                | any     | -                    | Only on Linux. Required for `install-xxx` to work.                                                         |
| [Homebrew](https://brew.sh/)                                         | any     | -                    | Only on macOS. Require for `install-xxx` to work.                                                          |

## Build Actions & Continuous Integration (GitHub Actions)

All of the following are part of the **build.sh** script and can be run locally on your system. Run `./build.sh` without parameters for details. The following table describes the status checks that are run with every pull request and master build:

| Check                       | Requirement                                                               | Description                                                                                                                                                                                                       | OS      | `build.sh`                               | Link                                                                 |
| --------------------------- | ------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------- | ---------------------------------------- | -------------------------------------------------------------------- |
| Build (**MSVC**)            | Compiles <br/> No warnings <br/> 100x Tests Pass <br/> 1x Benchmarks Pass | Windows MSVC x64 Release build. Runs unit tests 100 times to prevent flaky tests. Runs benchmarks once.                                                                                                           | Windows | `build` <br/> `tests` <br/> `benchmarks` | [MSVC](https://visualstudio.microsoft.com/cs/vs/features/cplusplus/) |
| Build (**GCC**)             | Compiles <br/> No warnings <br/> 100x Tests Pass <br/> 1x Benchmarks Pass | Linux GCC x64 Release build. Runs unit tests 100 times to prevent flaky tests. Runs benchmarks once.                                                                                                              | Linux   | `build` <br/> `tests` <br/> `benchmarks` | [GCC](https://gcc.gnu.org/)                                          |
| Build (**macOS**)           | Compiles <br/> No warnings <br/> 100x Tests Pass <br/> 1x Benchmarks Pass | macOS Clang x64 Release build. Runs unit tests 100 times to prevent flaky tests. Runs benchmarks once.                                                                                                            | Any     | `build` <br/> `tests` <br/> `benchmarks` | [Clang](https://clang.llvm.org/)                                     |
| Build (**Vue.js**)          | Compiles <br/> Tests pass                                                 | Build Vue.js applications and run the tests.                                                                                                                                                                      | Any     | `build-vuejs` <br/> `tests-vuejs`        | [Vue.js](https://vuejs.org/)                                         |
| Analysis                    | No warnings                                                               | Clang-Tidy static analysis. Runs on all C++ sources except third party code. Suppress undesirable warnings and diagnostics with `NOLINT`.                                                                         | Any     | `analysis`                               | [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/)               |
| Coverage                    | 100 % line <br/> 100% function any% region                                | LLVM code coverage. Region (branch) coverage is any% due to the tooling's inability to handle `constexpr`.                                                                                                        | Any     | `coverage`                               | [LLVM Cov](https://clang.llvm.org/docs/SourceBasedCodeCoverage.html) |
| Documentation               | Full documentation                                                        | Doxygen code documentation. All public/protected entities must be documented unless they are explicitly part of the implementation detail (guard these with `//! \cond IMPLEMENTAION_DETAIL` and `//! \endcond`). | Any     | `documentation`                          | [Doxygen](https://www.doxygen.nl/index.html)                         |
| Formatting                  | Everything formatted                                                      | Clang-Format formatting tool. Runs on all C++ sources except third part code. Use `// clang-format off` and `// clang-format on` to selectively suppress the automatic formatting.                                | Any     | `formatting`                             | [Clang-Format](https://clang.llvm.org/docs/ClangFormat.html)         |
| Sanitize Address            | No bug                                                                    | Clang's Address Sanitizer. Builds with Clang & runs tests.                                                                                                                                                        | Linux   | `sanitize-address`                       | [ASAN](https://clang.llvm.org/docs/AddressSanitizer.html)            |
| Sanitize Memory             | No bug                                                                    | Clang's Memory Sanitizer. Builds with Clang & runs tests. Downloads and builds LLVM/Clang `libc++` that is required for this sanitizer.                                                                           | Linux   | `sanitize-memory`                        | [MEMSAN](https://clang.llvm.org/docs/MemorySanitizer.html)           |
| Sanitize Undefined Behavior | No bug                                                                    | Clang's Undefined Behavior Sanitizer. Builds with Clang & runs tests.                                                                                                                                             | Linux   | `sanitize-ub`                            | [UBSAN](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) |

## Development

#### C++

Open `CMakeLists.txt` in your favorite IDE.

#### Vue.js

Choose from [Vue.js Dev Tools](https://vuejs.org/v2/guide/installation.html).
