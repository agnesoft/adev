# awinapi

-   [Overview](#overview)
-   [Prerequisites](#prerequisites)
-   [Usage](#usage)
-   [Build](#build)
-   [MSVC Warnings](#msvc-warnings)

## Overview

The `awinapi` module is a convenience wrapper around Windows SDK. It converts `Windows.h` into a header unit and re-exports it into a new module. Additionally it provides several utility wrappers and functions that make working with WinAPI easier.

## Prerequisites

-   C++ compiler
-   [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
-   [astl](projects/astl/readme.md)

## Usage

```
import awinapi;

std::string buffer(32767, char{});
::GetEnvironmmentVariable("PATH", buffer.datra(), static_cast<DWORD>(buffer.size()));
```

## Build

The build is the same as with [astl](projects/astl/readme.md#build) requiring first to convert the `windows.hpp` into a header unit and then compiling the module itself.

## MSVC Warnings

The Windows SDK (as of 03/11/2021, version 20348) headers rely on legacy preprocessor behavior that is being flagged by MSVC when conforming mode is enabled which is enforced when using C++ modules. These warnings also "bleed through" to the header units and to the users of them even if the usage is indirect (importing a module that imports the header unit or another module that imports it). Specifically it is these warnings:

-   4005: macro identifier is defined twice
-   5105: macro expansion producing 'defined' has undefined behavior
-   5106: macro redefined with different parameter names

In order to avoid having to suppress these warnings on every import transitionally it is required to disable these globally. Fortunately none of these warnings should affect the code that should be written macro-free.
