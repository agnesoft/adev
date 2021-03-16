# High Level Design

- [Problem](#problem)
- [Requirements](#requirements)
- [Existing Solutions](#excisting-solutions)
- [ABuild](#abuild)

## Problem

Building C++ is difficult.

There are only two steps to translating C++ sources into binary - compilation and linking. The compilation step takes a single source file (called [translation unit](https://en.wikipedia.org/wiki/Translation_unit_(programming))) and compiles it into a single binary object file. A collection of these object files is then "linked" together into a final binary - either executable (application), dynamic library (to be loaded during runtime) or a static library (to be linked in during compile time). Compiler toolchains implement exactly this build model.

The C++ build model has the following main challenges:

1. There is no way for individual translation units to communicate with each other or to share data. To work around this issue the preprocessor is used. During preprocessing of a source file we embed a different shared file (traditionally called "header") into each translation unit that needs to share the same data. The headers contain mainly declarations of symbols that are shared and used between the translation units (finding these symbols' definitions is the job of the linker). There are many issues with this workaround but until C++20 modules there was literally no other way.

2. It is possible to enable/disable sections of the code using preprocessor. Compilation of every translation unit is entirely independent so even though the headers are "shared" they may be percieved differently in different translation units. Produced binary object files might therefore have different notion of the symbols from the header file than others even though the code was "the same".

3. Different compiler settings can be used to compile each and every translation unit. This means that the produced binary object files can be incompatible with each other on the binary level due to differing settings. These are called [ABI (Application Binary Interface)](https://en.wikipedia.org/wiki/Application_binary_interface) breaks and include for example calling conventions, name mangling, exception settings etc. The linker has only very limited ways of detecting these incompatibilities and thus might produce crashing binary that is very hard to debug. The only way to guarantee the binary compatibility is to use the very same compiler settings for all translation units that are to be linked together into a final binary.

4. Dependencies are specified multiple times. Once in the code when we include a header or import a module. Then during compilation when we need to supply locations of the headers and module interfaces. And finally during linking we need to supply the libraries (or other object files) that contain the symbols used via these headers and modules.

Authoring calls to the compiler for every source file and then the linker call for every output binary is tedious, error prone and hard to maintain. Especially when targeting multiple compilers and platforms and their combinations. Using third party dependencies is also a significant challenge. Using a compiler toolchain directly is thus not feasible but for a simplest of projects.

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

- All of the declarative build systems offer multiple targets and configurations and offer compiler toolchain usage consistency. 
- Some of the build systems provide transparency so that the actual commands run against the compiler toolchain can be inspected.
- None of the existing build systems can run without prior configuration of the project in form of project/build files (often using other languages).
- None of the existing build systems support automatic dependency resolution - all of them require manual setup of dependencies often in multiple steps: introducing the dependency in the sources, setting up include directories in the build system for a given project and finally adding the dependency to another target.
- None of the existing build systems currently support C++20 modules.
- C++ build systems oddly enough are mostly not written in C++.

## ABuild

The **Agnesoft Build** or **ABuild** is a C++ build system. It provides fully automatic project detection including dependencies based on source inspection. The goal is to be able to run without any configuration of any kind by adhering to a well defined project structure instead. By ivoking `abuild` in a project root directory it shall detect all targets, their dependencies and build them using sensible defaults for an available compiler toolchain.

Usage example:

Default behaviour
```
abuild
```

Overriding configuration on the command line
```
abuild "{ 'Toolchain': { 'name': 'clang' } }"
```

### Environment Scanner

The `abuild` will attempt to find the compiler toolchain in its typical location on the current host and to verify that it is able to use it to produce a binary.

### Project Structure

The `abuild` will detect all translation units and all headers starting from the current working directory. The translation units will be analyzed for includes and imports. They will then be divided into `projects` based on the directories they are in. The projects will represent a binary output that will be inferred based on the file analysys (e.g. `ABuild/main.cpp` -> executable called `ABuild`).

### Dependency Resolution

After the project analysis the dependency resolution will try to find each of the included file (in case of headers) or imported module (in case of modules) and establish dependencies between each translation unit. It will also try to establish dependencies on the third-party libraries if known and found on the system.

### Configuration

By default no configuration of any kind shall be required. Sensible defaults will be used for building. Optionally a configuration file and/or command line arguments can be used to override these defaults and to achieve greater control over the build process.
