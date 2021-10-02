# adev

[![adev](https://github.com/agnesoft/adev/actions/workflows/adev.yml/badge.svg)](https://github.com/agnesoft/adev/actions/workflows/adev.yml)

Agnesoft central development repository.

## Projects

| Name                                            | Path                                              | Description                                  |
| ----------------------------------------------- | ------------------------------------------------- | -------------------------------------------- |
| [acommandline](projects/acommandline/readme.md) | [projects/acommandline/](/projects/acommandline/) | c++ command line parser                      |
| [atest](projects/atest/readme.md)               | [projects/atest/](/projects/atest/)               | c++ testing framework                        |
| [astl](projects/astl/readme.md)                 | [projects/astl/](/projects/astl/)                 | c++ standard template library module wrapper |

## Prerequisites

### Windows

-   MSVC 2019 (16.10+)

\- or -

-   LLVM 13+ (see [Known Issues](#known-issues))

### Linux

-   Clang 13

\- or -

-   GCC 11 (see [Known Issues](#known-issues))

## Usage

Run (use Git Bash on Windows):

```
./adev.sh
```

## Known Issues

-   _Windows_: Clang 12+ (LLVM) is unable to use MSVC's STL failing the build. [31/08/2021]
-   _Linux_: GCC 11 is unable to use its own STL via header units failing the build. [31/08/2021]
-   _Coverage_: LLVM instrumentation has difficulties with `if constexpr` and private module entities that means certain parts of code seem "uncovered" even though they are executed [29/09/2021]
