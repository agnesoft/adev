# High Level Design

-   [Overview](#overview)
-   [Context](#context)
    -   [Build Files](#build-files)
    -   [Preprocessor](#preprocessor)
    -   [ABI](#abi)
    -   [Existing Solutions](#existing-solutions)
-   [Requirements](#requirements)
-   [ABuild](#abuild)
    -   [Usage](#usage)
    -   [Command Line](#command-line)
    -   [Toolchain Scanner](#toolchain-scanner)
    -   [Package Scanner](#package-scanner)
    -   [Project Scanner](#project-scanner)
    -   [Translation Unit Analyzer](#translation-unit-analyzer)
    -   [Dependency Resolver](#dependency-resolver)
    -   [Build Cache](#build-cache)
    -   [Configuration](#configuration)
    -   [Build](#build)
    -   [Custom Commands](#custom-commands)
    -   [Build Installation](#build-installation)
    -   [Bootstrapping](#bootstrapping)

## Overview

The **Agnesoft Build** (**ABuild** or **abuild**) is a fully automatic C++ build system. It automatically detects and builds projects from C++ source files. By invoking `abuild` in a project root directory it detects all targets and their dependencies and build them using sensible defaults and an available (and also detected) compiler toolchain.

## Context

### Build files

All existing build systems require creation and maintenance of build files that describe what and how should be built. This information is duplicated however as it is usually already expressed in the source files themselves in form of source code organization and in the code itself.

Consider:

```
//MyApp/main.cpp
#include <iostream>
int main() { std::cout << "Hello, World!\n"; }
```

A file called `main.cpp` indicates it is a C++ source file (customary `.cpp` extension) and the resulting binary is to be an executable (customary `main` name). It is placed in a directory `MyApp` that indicates a name of the binary. The source file includes `<iostream>` which is a known STL header. In conclusion the author of this project clearly wants to build an executable called `MyApp` from the source file `main.cpp` that depends on STL.

And yet every existing C++ build system requires another file - a build file - that will repeat exactly this information that is already there in order to work and to produce correct output. This means double maintenance and leads to divergence of the code and the build files over time (e.g. stale/unnecessary dependencies). Furthermore it accommodates ambiguous and obscure code. There is no reason to use the `.cpp` extension or `main` as a main source file for executables, nor is it needed to name the containing directory after the binary being built. Most projects do this but plenty do not and even the most diligent will introduce subtle diversities (e.g. directory `MyServerApp` actually building executable called `MyServer`).

### Preprocessor

Another reason for build files is the preprocessor. The C++ preprocessor is unique in that a simple and arbitrary argument passed to the compiler can completely alter the code. From the compiler perspective the source files are incomplete "templates" that become real source files only after being preprocessed. While preprocessor is mostly used for including headers and thus sharing (kind of) information between source files, it can also be used to enable/disable code and to alter its meaning. That being said the preprocessor is necessary in C++ and it is mostly used reasonably.

Consider:

```
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
```

The dependency on `windows.h` (Windows SDK specific header) is conditional on `_WIN32` preprocessor define. The `_WIN32` is always defined when compiling for any Windows platform. Otherwise a dependency on POSIX threading header `pthread.h` is used when compiling for any other platform such as UNIX. Existing build systems deem code inspection a non-starter and thus cannot detect this at all. They rely on the user to tell them what preprocessor directives they are using for which configuration and what dependencies should the code be linked against for each of them. Then again the information has already been written by the user in the code though. They used a well known preprocessor define and they are depending on well known headers. It is clear that when a toolchain targeting Windows would be used (and thus `_WIN32` would be defined) the code should be linked against the Windows SDK and not against `pthread`. Conversely when a UNIX like system would be the target, the `_WIN32` would not be defined and the `pthread` should be used instead of Windows SDK.

### ABI

Last but not least of the concerns for C++ build automation is application binary interface or [ABI](https://en.wikipedia.org/wiki/Application_binary_interface). The C++ has unstable ABI and mixing binaries with different ABIs can be disastrous. The ABI is primarily determined by the compiler (it is typically not possible to mix C++ binaries from different compilers) and its version. Some compilers such as MSVC provide limited backwards compatibility (using older binaries from new ones). Another influence on ABI are compiler settings that should generally be consistent across all source files and certainly between binaries that are to be linked together or used during runtime together.

Build systems do mostly a good job in enforcing the compiler settings and preserving ABIs across built binaries. But it is also usually very easy to apply different settings down to the individual source file level. In theory everything should be built uniformly including all third party dependencies (that includes STL!). That is however not very practical (although some companies such as Google do so).

### Existing Solutions

There are many C++ build systems. They generally fall into two categories - imperative or declarative. The former expects the user to specify what & how to build. Their complexity is generally the same as that of using the compiler toolchain directly. The examples are [Make](<https://en.wikipedia.org/wiki/Make_(software)>) (`gmake`, `nmake` etc.) or [Ninja](<https://en.wikipedia.org/wiki/Ninja_(build_system)>). They are rarely used directly but are often produced by build generators.

Build generators are special kind of build systems that do not use the compiler toolchain directly but rather produce a build files in terms of other actual build system, often the imperative ones such as `Make` or `Ninja`. They abstract away the compiler toolchain complexity by leveraging other build systems for actual building. The drawback is that the build becomes rather opaque and might seem arbitrary at times as there are two extra layers between the programmer and the compiler toolchain. The examples of build generators are [CMake](https://en.wikipedia.org/wiki/CMake) or [Meson](<https://en.wikipedia.org/wiki/Meson_(software)>).

The declarative build systems organize translation units (and headers) into projects that typically correspond to the desired outputs (applications, libraries). The dependencies are then set manually between the individual projects. The build itself is orchestrated using generalized "rules" provided by the build system and applied to the project's source files. The user can customize the rules to a certain degree. Examples of declarative build systems are [Bazel](<https://en.wikipedia.org/wiki/Bazel_(software)>), [build2](https://build2.org/) or [boost.build (b2)](https://boostorg.github.io/build/).

There are attempts at creating a fully automatic build system that can in fact detect everything on its own and thus greatly simplifying the C++ build automation. Unfortunately there is no comprehensive and complete solution yet. An example of fully automatic build system is [Evoke](https://github.com/dascandy/evoke/tree/master).

With the advent of C++20 existing build systems struggle to provide support for features such as modules. C++20 modules significantly change the way C++ is built. Without real code inspection it is extremely difficult to provide support for them. The compiler toolchain themselves so far do not provide facilities that would let build systems know about available modules, module partitions etc. No build system so far provides even manual support for them except for [MSBuild](https://github.com/dotnet/msbuild) that is Windows and MSVC specific build system.

## Requirements

Goals:

-   Builds [C++](https://en.wikipedia.org/wiki/C%2B%2B) and is written in [C++](https://en.wikipedia.org/wiki/C%2B%2B).
-   Detects available compiler toolchains ([MSVC](https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B), [Clang](https://en.wikipedia.org/wiki/Clang), [GCC](https://en.wikipedia.org/wiki/GNU_Compiler_Collection)).
-   Detects available third party packages on the system ([STL](https://en.wikipedia.org/wiki/Standard_Template_Library), [Windows SDK](https://en.wikipedia.org/wiki/Microsoft_Windows_SDK), [Qt](<https://en.wikipedia.org/wiki/Qt_(software)>) etc.).
-   Detects projects to be built from the source file organization on the file system.
-   Detects both internal and external dependencies from code inspection observing preprocessor directives.
-   Allows building without any configuration or build files.
-   Enforces uniform compiler toolchain settings across all source files.
-   Allows overriding the detected values and compiler toolchain settings via an optional configuration file down to the file level.
-   Allows building for multiple architectures ([x86](https://en.wikipedia.org/wiki/X86), [x64](https://en.wikipedia.org/wiki/X86-64), [ARM](https://en.wikipedia.org/wiki/ARM_architecture)).
-   Allows building for multiple platforms ([Windows](https://en.wikipedia.org/wiki/Microsoft_Windows), [macOS](https://en.wikipedia.org/wiki/MacOS), [Linux](https://en.wikipedia.org/wiki/Linux), [Android](<https://en.wikipedia.org/wiki/Android_(operating_system)>)).
-   Allows building with different preset configurations (debug, release).
-   Allows inspection of the build commands.
-   Supports incremental build.
-   Supports parallel build.
-   Supports [C++20 modules](https://en.cppreference.com/w/cpp/language/modules).
-   Can bootstrap itself using supported compiler toolchains directly ([MSVC](https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B), [Clang](https://en.wikipedia.org/wiki/Clang), [GCC](https://en.wikipedia.org/wiki/GNU_Compiler_Collection)).

Future goals:

-   IDE support (e.g. [VSCode](https://en.wikipedia.org/wiki/Visual_Studio_Code), [Qt Creator](https://en.wikipedia.org/wiki/Qt_Creator), [Visual Studio](https://en.wikipedia.org/wiki/Microsoft_Visual_Studio), [CLion](https://en.wikipedia.org/wiki/JetBrains#CLion) etc.).
-   Support of precompiled headers.
-   Support of more configurations (e.g. coverage for [llvm-cov](https://llvm.org/docs/CommandGuide/llvm-cov.html) or [Gcov](https://en.wikipedia.org/wiki/Gcov), sanitizers such as [asan](https://clang.llvm.org/docs/AddressSanitizer.html)/[ubsan](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)/[memsan](https://clang.llvm.org/docs/MemorySanitizer.html)/[thread](https://clang.llvm.org/docs/ThreadSanitizer.html) that require code instrumentation).
-   Support of compiler toolchain tools (e.g. [clang-tidy](https://clang.llvm.org/extra/clang-tidy/), [clang-format](https://clang.llvm.org/docs/ClangFormat.html) etc.).
-   Support of other programming languages (e.g. [C](<https://en.wikipedia.org/wiki/C_(programming_language)>), [C#](<https://en.wikipedia.org/wiki/C_Sharp_(programming_language)>) etc.).
-   Support of other compiler toolchains (e.g. [Intel](https://en.wikipedia.org/wiki/Intel_C%2B%2B_Compiler), [MinGW](https://en.wikipedia.org/wiki/MinGW) etc.).
-   Support of other platforms and architectures (e.g. [iOS](https://en.wikipedia.org/wiki/IOS), [ChromeOS](https://en.wikipedia.org/wiki/Chrome_OS), [WebAssembly](https://en.wikipedia.org/wiki/WebAssembly) etc.).
-   Build generator for other build systems/generators (e.g. [CMake](https://en.wikipedia.org/wiki/CMake), [MSBuild](https://en.wikipedia.org/wiki/MSBuild), [Make](<https://en.wikipedia.org/wiki/Make_(software)>))

Non goals:

-   Support of multiple configuration files or their hierarchy.

## ABuild

### Usage

```
abuild [options]
```

By default no arguments or options are necessary and the `abuild` will build all projects using the default toolchain (see below) for the given host and it will target the same host. The default configuration is `release`:

-   Configuration: `release`
-   Windows: `MSVC` (`Clang`)
-   Linux: `GCC` (`Clang`)
-   macOS: `Clang`

### Command Line

-   `--toolchain=<name> -t=<name>` selects by partially matching a toolchain for use to build (e.g. `-t=clang-11`).
-   `--configuration=<name> -c=<name>` selects a configuration. Available options: `release`, `debug` (e.g. `-c=debug`).
-   `--project=<name> -p=<name>` selects a subset of projects to be built. Accepts wildcard `*` (e.g. `-p=some_project`, `-p=*_test`).
-   `--file=<path> -f=<path>` compiles only selected file including its dependencies such as modules it uses (e.g. `-f=/path/to/source.cpp`).

### Toolchain Scanner

The toolchain scanner attempts to find compiler toolchains in their typical/default locations on the current host. The main supported compilers are:

-   MSVC (Windows)
-   Clang (Windows, Linux, macOS)
-   GCC (Linux)

Information required for each toolchain:

-   Name (family-version-target_architecture-full_version)
    -   family: `msvc`/`clang`/`gcc`
    -   version: (e.g. `2019`, `11`)
    -   target_architecture: `x86`/`x64`/`ARM`
    -   full_version: (e.g. `14.29.30037`, `11.1`)
-   Compiler full path (e.g. `/usr/bin/g++`)
-   Linker full path (e.g. `/usr/bin/ld`)
-   Archiver full path (e.g. `/usr/bin/ar`)
-   ABI:
    -   Type: `msvc`/`clang`/`gcc`
    -   Target architecture: `x86`/`x64`/`ARM`
    -   Major version (e.g. `19`)
    -   Minor version (e.g. `29`)
    -   Build version (e.g. `30037`)
    -   Preprocessor defines (e.g. `_WIN32=1`, `_MSC_VER=1930`, `__clang__`)
    -   Compiler flags (e.g. `/nologo`, `/std:c++20`, `/EHsc`)
    -   Linker flags (e.g. `/NOLOGO`)
    -   Archiver flags (e.g. `/NOLOGO`)

Toolchains are output to the [build cache](#build-cache). Toolchains already in the cache are checked whether they exist on startup. Toolchains can be also addded manually via [configuration file](#configuration). The lookup is done by partial name matching. For example if there are two toolchains `clang-11` and `clang-10` then requesting simply `clang` will yield the highest version of that compiler: `clang-11`.

### Package Scanner

The package scanner attempts to find third party packages in their typical default locations on the current host and will scan their content. The supported packages are:

-   Compiler intrinsic
-   STL
-   C standard library
-   Windows SDK
-   Qt
-   Linux system headers & libraries

Information required for each package:

-   Name
-   Version in format `major[.minor[.patch[.build]]]`
-   [ABI](#toolchain-scanner)
-   Headers (list of files)
-   Modules (list of compiled module interface files)
-   Libraries (list of binary libraries)

### Project Scanner

The project scanner will detect all translation units and all headers starting from the current working directory. The translation units will be analyzed for includes and imports. They will then be divided into `projects` based on the directories they are in. The projects will represent a binary output that will be inferred based on the file analysis (e.g. `abuild/main.cpp` -> executable called `abuild`).

Rules for file detection:

-   Translation unit: `\.c(pp|xx|c)$/i`.
-   Header unit: `\.h(|pp|xx)$/i`.
-   Module interface unit: `\.(i|m)(xx|pp)$/i`.

Rules for project detection:

-   Any directory containing at least one of the C++ files described above is a project including the root directory.
-   The `^src$/i`, `^include$/i` are considered subdirectories of their respective parent project.
-   The `<parent>/include/<project>` is considered a subdirectory of a project if `<parent>` is equal to `<project>`.
-   The `^test$/i` will produce an independent project called `<parent>test`.
-   If a project contains another project its name will be concatenated as `<parent>.<project>`.

Rules for project types:

-   Project containing `^main\.c(c|pp|xx)$/i` is an **executable**.
-   Project containing any translation unit is a **static  library**.
-   Project containing only header files is a header only library (no build).
-   Project fulfilling custom rules for dynamic library is a **dynamic library**.

The output of the project scanner should be the list of translation units and the list of projects.

### Translation Unit Analyzer

The translation unit analyzer will perform basic analysis of each of the translation unit, header and module interface. It will extract primarily `#include`, `export module` and `import module` directives and augment the information about each translation unit with it. The LLVM Clang should be used for performing this analysis.

### Dependency Resolver

The dependency resolver will try to find each of the included file (in case of headers) or imported module (in case of modules) and establish dependencies between the translation units. Standard library headers shall be found within the STL used for building. Third-party dependencies will be looked for in the well-known locations (e.g. `/usr/lib` on Unix systems).

The dependencies between the units will be recorded in the information about each translation unit.

### Build Cache

All the information detected and used by the `abuild` will be recorded in a single build cache file. The file will be a JSON file with the following sections:

-   **Rules**: rules used for detecting toolchains and projects.
-   **Toolchains**: list of detected C++ compiler toolchains along with information needed to invoke them including STL library (or libraries) found. Default configuration settings should be provided. Default configurations are:
        -  `release` (default): full optimization for speed, 
        -  `debug`: no optimization, debugging symbols enabled
        -  Common settings: exceptions enabled, RTTI enabled, C++20 enabled
-   **Files**: list of C++ files detected with meta information from the analysis and dependency resolving.
-   **Projects**: list of detected projects, their type and collection of files they contain.

The build cache file will be produced in the root of the build directory.

Optionally a build commands JSON file containing actual commands issued to a compiler toolchain can be produced from the build cache as well to the root of the build directory.

### Configuration

By default, no configuration of any kind shall be required. Sensible defaults will be used for building. Optionally a configuration file and/or command line arguments can be used to override these defaults and to achieve greater control over the build process. The configuration file has the same syntax and structure as the build cache and is applied over it. It is therefore possible to override any part of the build cache thus adding new custom configurations, resolving undetected dependencies, applying additional or different compiler flags down to the file level etc.

The configuration file must be in the root of the working directory with the suffix `.abuild`.

### Build

The build is done based on the dependency graph produced from the build cache in the "shadow" directory (same structure as the project itself) named by the toolchain and the configuration being built. The translation units will be built in parallel. A project should be linked as soon as all its translation units (and their dependencies) are built. All built dynamic libraries and executables shall be placed in `<build directory>/bin`.

### Custom Commands

Before and after each build step (compilation, linking) as well as before and after the entire build there can be custom command(s) specified to be run. For example to generate source files, support COMs etc.

### Build Installation

By default, no installation is done. It can be enabled and customized.

### Bootstrapping

Bootstrapping is done for known hard-coded compiler toolchains. These are used directly using OS specific native shell script (bash/cmd) to build `abuild` first. Built `abuild` will be able to build itself again if desired or be directly used as described.
