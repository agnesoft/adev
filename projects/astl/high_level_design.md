# High Level Design

-   [Problem](#problem)
-   [Requirements](#requirements)
-   [Existing Solutions](#existing-solutions)
-   [ASTL](#astl)

## Problem

Using C++ Standard Template Library (STL) as C++20 modules is not possible, yet STL is essential part of most of C++ software. Using direct textual includes of STL headers within modules or their users is not possible either as it leads to compiler errors. Generating `header units` (kind of precompiled header into module format) on demand also does not work for all STL headers due to their overlaps.

## Requirements

-   Ability to use STL with C++20 modules.

## Existing Solutions

Currently, the only modularized version of STL is available as experimental feature with MSVC. This Microsoft's modularized version of STL does not work very well since one cannot import together all parts of it (e.g. `std.core` and `std.filesystem`). Furthermore some symbols seem to be simply broken, particularly around `I/O` part of the library.

An alternative option could be to modularize individual headers from MSVC STL as they are needed. Some headers however overlap (i.e. `iostream` and `sstream`) and due to the requirements of isolation of modules and header units they simply cannot be imported together. Other headers seem to be sensitive to the order of imports since the symbols are used in different header units that however both overlap and are isolated. Re-exporting them also seems to be an issue.

The header units creation and usage is also not consisent across compiler toolchains. For example they require to be manually created and supplied with MSVC but module map file with clang.

## ASTL

Agnesoft Standard Template Library (ASTL) is a convenient modules-aware wrapper around STL. It consists of a single header file that includes entire STL and is to be converted into a single `header unit`. Since the header uses textual includes there should be no overlaps and all symbols should be present and exported correctly into a single header unit that exposes everything available within STL.
