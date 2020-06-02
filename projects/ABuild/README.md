# ABuild

An automatic C++ build system.

## High Level Design

C++ has a diverse ecosystem of compilers, build systems and other tools. Due to a relative simplicity of C++ build model compiler toolchain should be enough to build C++ code and produce binaries. However authoring compiler command lines can be tricky and cumbersome with all the different flags, include paths etc. Moreover every toolchain is different, using different flags and syntax making is especially hard to produce binaries cross platform. It essentially requires authoring the build commands as many times as there are platform/configuration combinations. Therefore many build systems were invented that abstract the user from authoring these command lines offering "simpler" way authoring only one set of them. Yet it is not possible for any build system to completely hide all the platform specific settings. Some build systems invent their own flags that acts like a middle man between one or many underlying toolchains, others simply let the user to pass any flag directly relying on if/else statements in their own language. Still virtually all build systems require a user to tell them what to actually do. Even for simplest of source code, even single source file, a user might need to write a build script in whatever build system of their choosing to get a binary out. This is in a sharp contrast with almost all other programming languages. Most of them, including compiled ones, simply don't have a build system or just have one. Most also don't require user to write anything apart from simply pointing the build system to a directory with code in it. **ABuild** is a build system that aims to provide just that for C++ - fully automatic build system that auto-detects everything and outputs the binaries.

### Build Model

C++ build model can be summarized as:

_Translation Unit (_.cc/_.cpp/_.cxx) ---compilation---> Object File (_.o/_.obj) ---linking---> Binary File (_lib/_.dll/_.exe)_

ABuild is extending the model by:

- Recognizing header files (_.h/_.hxx/\*.hpp)
- Adding projects that act as collections of translation units and to determine resulting binaries
- Auto-detecting dependencies between projects based on included header files (for both compilation & linking purposes)

The ABuild build model can be summarized as:

_Project Root Directory ---scanning---> Projects with Translation Units & Headers ---resolving---> Projects With Set Dependencies ---compiling---> Object Files ---linking---> Binary Files_

### Project

Project is a directory with source files to build. The name of the directory is the name of the project. The project has a type that determines the output binary file, one of:

- _Static Library_: Compiles into _.lib or _.a and is linked directly into other executables.
- _Dynamic Library_: Compiles into _.dll or _.so or \*.dylib and is loaded dynamically during runtime.
- _Header Only Library_: Is not compiled into anything but its files are used via preprocessor by other project types.
- _Executable_: Runnable binary application.

The type is auto-detected based on the scan of the source files (e.g. found `main.cpp` == `Executable`). Projects can have nested directories that will not act as projects but will merge with the parent project to help organize code. They are any directories with the following names:

- _project name itself_
- include/Include/includes/Includes
- src/SRC/Src/sources/Sources

Some directory names will create a compound project name, taking the parent project's name and appending its name to it:

- test/Test/tests/Tests

### Source File

Can be either translation unit (cpp file) or a header file. Both are recognized and added to their respective projects. Only translation units are compiled. The `#include` directives are extracted from all source files to resolve the dependencies between projects.

- Translation Units are any files with extension _.cpp/_.cc/\*.cxx
- Header files are any files with extension _.hpp/_.hxx/\*.h
