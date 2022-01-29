# yamlcpp

-   [Overview](#overview)
-   [Prerequisites](#prerequisites)
-   [Usage](#usage)
-   [Build](#build)

## Overview

C++20 module wrapper for [yaml-cpp](https://github.com/jbeder/yaml-cpp/).

## Prerequisites

-   C++ compiler

## Usage

```
import yamlcpp;
```

The usage is then the same as described in the readme of the [yaml-cpp](https://github.com/jbeder/yaml-cpp/).

## Build

Requires converting the `yaml.h` into the header unit and build the individual source files into a standalone library (including the new `yamlcpp.cpp` module source file).
