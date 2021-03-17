# High Level Design

- [Problem](#problem)
- [Requirements](#requirements)
- [Existing Solutions](#existing-solutions)
- [ABuild](#abuild)
    - [Environment Scanner](#environment-scanner)
    - [Project Scanner](#project-scanner)
    - [Translation Unit Analyzer](#translation-unit-analyzer)
    - [Dependency Resolver](#dependency-resolver)
    - [Build Cache](#build-cache)
    - [Configuration](#configuration-file)
    - [Command Line](#command-line)
    - [Build](#build)
    - [Custom Commands](#custom-commands)
    - [Build Installation](#build-installation)
    - [Bootstrapping](#bootstrapping)

## Problem

Building C++ is difficult.

There are only two steps to translating C++ sources into binary - compilation and linking. The compilation step takes a single source file (called [translation unit](https://en.wikipedia.org/wiki/Translation_unit_(programming))) and compiles it into a single binary object file. A collection of these object files is then "linked" together into a final binary - either executable (application), dynamic library (to be loaded during runtime) or a static library (to be linked in during compile time). Compiler toolchains implement exactly this build model.

The C++ build model has the following main challenges:

1. There is no way for individual translation units to communicate with each other or to share data. To work around this issue the preprocessor is used. During preprocessing of a source file we embed a different shared file (traditionally called "header") into each translation unit that needs to share the same data. The headers contain mainly declarations of symbols that are shared and used between the translation units (finding these symbols' definitions is the job of the linker). There are many issues with this workaround but until C++20 modules there was literally no other way.

2. It is possible to enable/disable sections of the code using preprocessor. Compilation of every translation unit is entirely independent so even though the headers are "shared" they may be perceived differently in different translation units. Produced binary object files might therefore have a different notion of the symbols from the header file than others even though the code was "the same".

3. Different compiler settings can be used to compile each and every translation unit. This means that the produced binary object files can be incompatible with each other on the binary level due to differing settings. These are called [ABI (Application Binary Interface)](https://en.wikipedia.org/wiki/Application_binary_interface) breaks and include for example calling conventions, name mangling, exception settings etc. The linker has only very limited ways of detecting these incompatibilities and thus might produce crashing binary that is very hard to debug. The only way to guarantee the binary compatibility is to use the very same compiler settings for all translation units that are to be linked together into a final binary.

4. Dependencies are specified multiple times. Once in the code when we include a header or import a module. Then during compilation when we need to supply locations of the headers and module interfaces. And finally during linking we need to supply the libraries (or other object files) that contain the symbols used via these headers and modules.

Authoring calls to the compiler for every source file and then the linker call for every output binary is tedious, error-prone and hard to maintain. Especially when targeting multiple compilers and platforms and their combinations. Using third party dependencies is also a significant challenge. Using a compiler toolchain directly is thus not feasible but for the simplest of projects.

## Requirements

The solution to the difficulty of building C++ is to have a build system that:

- Provides compiler toolchain settings consistency.  
- Provides automatic dependency resolution.
- Allows targeting multiple toolchains and platforms.
- Allows different configurations.
- Allow building without project configuration.
- Is transparent in what and how it does things.
- Is written in C++.
- Can bootstrap itself.
- Supports C++20 modules.

## Existing Solutions

There are many C++ build systems. They generally fall into two categories - imperative or declarative. The former expects the user to specify what & how to build. Their complexity is generally the same as that of using the compiler toolchain directly. The examples are [Make](https://en.wikipedia.org/wiki/Make_(software)) (gmake, nmake etc.) or [Ninja](https://en.wikipedia.org/wiki/Ninja_(build_system)). Their primary usage is to be produced by a build generator.

Build generators are special kind of build systems that does not use the compiler toolchain directly but rather produces a build in terms of other actual build systems, often the imperative ones such as Make or Ninja. They abstract away the compiler toolchain complexity by leveraging other build systems for actual building. The drawback is that the complexity is actually higher and the build become rather opaque and might seem arbitrary as there are two extra layers between the programmer and the compiler. The examples of build generators are [CMake](https://en.wikipedia.org/wiki/CMake) or [Meson](https://en.wikipedia.org/wiki/Meson_(software)).

The declarative build systems organize translation units (and headers) into projects that typically correspond to the desired outputs (applications, libraries). The dependencies are then set manually between the individual projects. The build itself is orchestrated using generalized "rules" provided by the build system and applied to the project's source files. Optionally user can customize the rules to a certain degree. Examples of declarative build systems are [Bazel](https://en.wikipedia.org/wiki/Bazel_(software)), [build2](https://build2.org/) or [boost.build (b2)](https://boostorg.github.io/build/).

- All the declarative build systems offer multiple targets and configurations and offer compiler toolchain usage consistency. 
- Only some build systems provide transparency so that the actual commands run against the compiler toolchain can be inspected.
- None of the existing build systems can run without prior configuration of the project in the form of project/build files (often using other languages).
- None of the existing build systems support automatic dependency resolution - all of them require manual setup of dependencies often in multiple steps: introducing the dependency in the sources, setting up include directories in the build system for a given project and finally adding the dependency to another target.
- None of the existing build systems currently support C++20 modules.
- C++ build systems oddly enough are mostly not written in C++.

## ABuild

The **Agnesoft Build** or **ABuild** is a C++ build system. It provides fully automatic project detection including dependencies based on source inspection. The goal is to be able to run without any configuration of any kind by adhering to a well-defined project structure instead. By invoking `abuild` in a project root directory it shall detect all targets, their dependencies and build them using sensible defaults for an available compiler toolchain.

Usage example:

Default behavior
```
abuild
```

Overriding configuration on the command line
```
abuild "{ 'Toolchain': { 'name': 'clang' } }"
```

### Environment Scanner

The environment scanner will attempt to find the compiler toolchain in its typical location on the current host and to verify that it is able to use it to produce a binary. The locations should be configurable (see [Configuration File](#configuration-file)) but it should not be required when everything is installed in standard paths. The main supported compilers are:

- MSVC (Windows)
- GCC (Linux)
- Clang (Windows, Linux, Unix)

The environment scanner should perform a basic check that the compiler found actually works and give an error if either no compiler could be found or if it does not appear to work (tested by compiling a basic program).

The output of this step should be entries in the [Build Cache](#build-cache) with the detected information (paths, default flags etc.).

### Project Scanner

The project scanner will detect all translation units and all headers starting from the current working directory. The translation units will be analyzed for includes and imports. They will then be divided into `projects` based on the directories they are in. The projects will represent a binary output that will be inferred based on the file analysis (e.g. `abuild/main.cpp` -> executable called `abuild`).

Rules for file detection:

- Translation unit: `\.c(pp|xx|c)$/i`.
- Header unit: `\.h(|pp|xx)$/i`.
- Module interface unit: `\.(i|m)(xx|pp)$/i`.

Rules for project detection:

- Any directory containing at least one of the C++ files described above is a project including the root directory.
- The `^src$/i`, `^include$/i` are considered subdirectories of their respective parent project.
- The `<parent>/include/<project>` is considered subdirectory of a project if `<parent>` is equal to `<project>`.
- The `^test$/i` will produce an independent project called `<parent>test`.
- If a project contains another project its name will be concatenated as `<parent>.<project>`.

Rules for project types:

- Any project containing `^main\.c(c|pp|xx)$/i` is an **executable**.
- Any other project containing any translation unit is a **static  library**.
- Any project containing only header files is a header only library (no build).
- Any project fulfilling a custom rule for dynamic library is **dynamic library**.

The output of the project scanner should be translation units list and the project list containg the translation units.

### Translation Unit Analyzer

The translation unit analyzer will perform basic analysis of each of the translation unit, header and module interface. It will extract primarily `#include`, `export module` and `import module` directives and augment the information about each translation unit with it. The LLVM Clang should be used for performing this analysis.

### Dependency Resolver

The dependency resolver will try to find each of the included file (in case of headers) or imported module (in case of modules) and establish dependencies between the translation units. Standard library headers shall be found within the STL used for building. Third-party dependencies will be looked for in the well-known locations (e.g. `/usr/lib` on Unix systems).

The dependencies between the units will be recorded in the information about each translation unit.

### Build Cache

All of the information detected and used by the `abuild` will be recorded in a single build cache file. The file will be a JSON file with following sections:

- **Rules**: rules used for detecting toolchains and projects.
- **Toolchains**: list of detected C++ compiler toolchains along with information needed to ivoke them including STL library (or libraries) found. Default configuration settings should be provided. Default configurations are:
    -  `release` (default): full optimization for speed, 
    -  `debug`: no optimization, debugging symbols enabled
    -  Common settings: exceptions enabled, RTTI enabled, C++20 enabled
- **Files**: list of C++ files detected with meta information from the analysis and dependency resolving.
- **Projects**: list of detected projects, their type and collection of files they contain.

The build cache file will be produced in the root of the build directory.

Optionally a build commands JSON file containing actual commands issued to a compiler toolchain can be produced from the build cache as well to the root of the build directory.

### Configuration File

By default, no configuration of any kind shall be required. Sensible defaults will be used for building. Optionally a configuration file and/or command line arguments can be used to override these defaults and to achieve greater control over the build process. The configuration file has the same syntax and structure as the build cache and is applied over it. It is therefore possible to override any part of the build cache thus adding new custom configurations, resolving undetected dependencies, applying additional or different compiler flags down to the file level etc.

The configuration file must be in the root of the working directory with the suffix `.abuild`.

### Command Line

The command line parameters should allow:

- Selecting a toolchain. By default, the first one in the list is used. By supplying `--toolchain=<name> -t=<name>` one can select a different detected (or configuration supplied) toolchain.
- Selecting a configuration. By default, the first one in the list for a given toolchain is used. By supplying `--configuration=<name> -c=<name>` one can select a different configuration.
- Building a subset of the project. By default, everything is built. By supplying a subdirectory or a single file (or their list) only the subset will be build (however analysis will still be performed for the entire tree for dependencies etc. Syntax: `--path=<relative path> -p=<relative path>`.
- Overriding configuration by supplying a JSON string as a positional argument that will take precedence over the file configuration (if any) and build cache. E.g. `abuild "{ ... }"`.

### Build

The build is done based on the dependency graph produced from the build cache in the "shadow" directory (same structure as the project itself) named by the toolchain and the configuration being built. The translation units will be built in parallel. A project should be linked as soon as all its translation units (and their dependencies) are built. All built dynamic libraries and executables shall be placed in `<build directory>/bin`.

### Custom Commands

Before and after each build step (compilation, linking) as well as before and after the entire build there can be custom command(s) specified to be run. For example to generate source files, support COMs etc.

### Build Installation

By default no installation is done. It can be enabled and customized.

### Bootstrapping

Bootstrapping is done for known hard-coded compiler toolchains. These are used directly using OS specific native shell script (bash/cmd) to build `abuild` first. Built `abuild` will be able to build itself again if desired or be directly used as described.
