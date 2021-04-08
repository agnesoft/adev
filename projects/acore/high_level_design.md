# High Level Design

-   [Problem](#problem)
-   [Requirements](#requirements)
-   [Existing Solutions](#existing-solutions)
-   [ACore](#acore)
    -   [CommandLine](#commandline)

## Problem

There is a common need for generic functionality (command line handling, logging etc.) that is required by most projects and that is not readily available in Standard Template Library (STL) that ships with every major compiler toolchain.

## Requirements

-   Used as C++20 module.
-   Cross platform (Linux, Windows, Unix).
-   Cross platform API (no platform specifics in the API).

## Existing Solutions

There are plenty of cross platform libraries. `Qt 6` satisfies the cross platformness and cleanness of API the most. However it is strictly C++17 and some of its design choices (inheritance and virtual calls) are outdated. `Poco` is another option but it is in many areas incomplete and requires users to implement parts of the API themselves. It is also only C++14. `Boost` is probably most widely used library beside STL and often serves as a staging ground for official STL features. The STL API itself (and conversely, Boost's API) is less than ideal and Boost is often even worse. This makes Boost extremely hard to understand and to use. Its quality and maintanance varies from one library to the next with many parts being basically abandoned. The issue with the large library collections is also that they contain lots of stuff that is not really needed. It becomes an overkill to start using Qt or Boost to make use of one or two features.

Aside from huge libraries there are standalone solutions for each problem. They tend to differ a lot in all aspects and would require wrappers to bring them to the common standard. Nevertheless some standalone libraries are such high quality and with superb maintenance that implementing them anew would be folly (e.g `rapidjson`).

## ACore

Agnesoft Core is the base library written in C++20 for all other projects that provide generic functionality with consistent API. It does utilize third party libraries and abstract the users from their specific APIs by providing a wrapper. The `acore` is consumed as a module.

### CommandLine

Command line parser taking standard arguments `int` and `*char[]` and parsing them into variables using options defined with builder pattern.

**Requirements**

-   Parsing directly into variables.
-   Options defined with builder pattern.
-   Single style of command line options.
-   Error handling.
-   Automatic help.

**Existing Solutions**

-   `Boost.Program_options` lacks builder pattern and storing the options in user's code. The API is horrible.
-   `GNU getopt` has C API.
-   Qt's `QCommandLine` requires `QCoreApplication` and generally must be used within Qt framework.
-   `cxxopts` lacks builder pattern and stores options only internally but has strong typing. The API is rather weird requiring passing internal value classes when defining options plus still requires casting when retrieving values.

**Design**

Example:

```
std::string input;
bool flag = false;
std::string output;
std::vector<std::string> includePaths;

acore::CommandLine commandLine;
commandLine.option().longName("input").shortName('i').required().description("The required input file").bindTo(&input);
commandLine.option().longName("flag").description("An optional flag.").bindTo(&flag);
commandLine.option().positional().defaultValue(std::string{"a.o"}).description("An optional defaulted output file.").bindTo(&output);
commandLine.option().longName("Include").description("Optional repeatable argument").bindTo(&includePaths);

commandLine.parse(argc, argv);
```

Supported types:

-   `std::string`
-   `std::int64_t`
-   `double`
-   `std::vector<std::string>`
-   `std::vector<std::int64_t>`
-   `std::vector<double>`

Supported syntax:

-   Long name is mandatory and must match the pattern `[a-zA-Z][a-za-z\\d]+` and always start with double dash, e.g. `--longName1`
-   Short name is optional and must be a single alphabetic character `[a-zA-Z]` and always start with a single dash, e.g. `-l`
-   Value can be provided using equal sign, e.g. `--longName1=value`
-   Value can be provided as a next argument, e.g `-l value`
-   Positional arguments are completely freeform
-   Quoted values are unquoted
-   Each option can match at most once unless it is bound to repeatable value, e.g. acore::Vector

The parsing shall fail and throw an exception if:

-   Required argument was not provided
-   Value was not provided for a named option
-   Type mismatch of provided value and the argument (e.g non-integer string to an integer value)
-   Unknown/unmatched argument
-   Syntax error (usually interpreted as missing value or value of incorrect type)
