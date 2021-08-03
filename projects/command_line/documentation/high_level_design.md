# High Level Design - command_line

This document outlines context, requirements and design specification of the `command_line` project.

-   [Problem](#problem)
-   [Requirements](#requirements)
-   [Existing Solutions](#existing-solutions)
-   [command_line](#commandline)
    -   [Usage](#usage)
    -   [Supported Types](#supported-types)
    -   [Syntax](#syntax)
    -   [Errors](#errors)

## Problem

Most applications do need to handle and use input arguments but C++ Standard Template Library (STL) does not provide any standard way of doing that. Manually parsing the arguments from the raw arguments is tedious, requires manual type checking and error handling and is generally error prone.

## Requirements

-   Parsing directly into variables.
-   Options defined with builder pattern.
-   Single style of command line options.
-   Error handling.
-   Automatic help.

## Existing Solutions

-   `QCommandLine` is the best out there but requires Qt and is licensed under LGPL.
-   `Boost.program_options` has horrible API a bleeds implementation to the user code.
-   `GNU getopt` has C API.
-   `cxxopt` makes it quite hard to retrieve the values requiring use of internal types and casting.
-   `CLIUtils` does not output options to user variables and uses macros but is very feature rich.
-   Many more parsers out there suffer from similar issues as they are either part of the larger framework or have defective API.

## command_line

C++ Command line parser taking standard arguments `int` and `*char[]` and parsing them into variables using options defined via builder pattern.

### Usage

```
std::string input;
bool flag = false;
std::string output;
std::vector<std::string> include_paths;

adev::command_line cmd;
cmd.option().long_name("input").short_name('i').required().description("The required input file").bind_to(&input);
cmd.option().long_name("flag").description("An optional flag.").bind_to(&flag);
cmd.option().positional().default_value(std::string{"a.o"}).description("An optional defaulted output file.").bind_to(&output);
cmd.option().long_name("include").description("Optional repeatable argument").bind_to(&include_paths);

cmd.parse(argc, argv);
```

### Supported types

-   `std::string`
-   `std::int64_t`
-   `double`
-   `std::vector<std::string>`
-   `std::vector<std::int64_t>`
-   `std::vector<double>`

### Syntax:

-   Long name is mandatory and must match the pattern `[a-zA-Z][a-za-z\\d]+` and always start with double dash, e.g. `--longName1`
-   Short name is optional and must be a single alphabetic character `[a-zA-Z]` and always start with a single dash, e.g. `-l`
-   Value can be provided using equal sign, e.g. `--longName1=value`
-   Value can be provided as a next argument, e.g `-l value`
-   Positional arguments are completely freeform
-   Quoted values are unquoted
-   Each option can match at most once unless it is bound to repeatable value, e.g. acore::Vector

### Errors

An exception with details about an error is thrown when:

-   Required argument was not provided
-   Value was not provided for a named option
-   Type mismatch of provided value and the argument (e.g non-integer string to an integer value)
-   Unknown/unmatched argument
-   Syntax error (usually interpreted as missing value or value of incorrect type)
