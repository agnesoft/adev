# ACore

C++ core library that provides generic functionality.

-   [Dependencies](#dependencies)
-   [Reference](#reference)
    -   [CommandLine](#commandline)
-   [Known Issues](#known-issues)
    -   [Affects Users](#affects-users)
    -   [Does Not Affect Users](#does-not-affect-users)

## Dependencies

-   C++20
-   [ASTL](../astl/README.md)

## Reference

### CommandLine

Strongly typed command line parser that takes standard arguments `int` and `*char[]` and parses them into user variables according to options defined by the user with the builder pattern.

Example:

```
//int main(int argc, char *argv[])
//argc: "./app.exe", "-i", "main.cpp", "--flag", "--Include=/some/path", "--Include=\"other/path/with space/\"
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

//intput == main.cpp
//flag == true
//output == a.o
//includePaths == {/some/path, other/path/with space/}
```

**Supported types**

-   `std::string`
-   `std::int64_t`
-   `double`
-   `std::vector<std::string>`
-   `std::vector<std::int64_t>`
-   `std::vector<double>`

**Supported syntax**

-   Long name is mandatory and must match the pattern `[a-zA-Z][a-za-z\\d]+` and always start with double dash, e.g. `--longName1`
-   Short name is optional and must be a single alphabetic character `[a-zA-Z]` and always start with a single dash, e.g. `-l`
-   Value can be provided using equal sign, e.g. `--longName1=value`
-   Value can be provided as a next argument, e.g `-l value`
-   Positional arguments are completely freeform
-   Quoted values are unquoted
-   Each option can match at most once unless it is bound to repeatable value, e.g. `acore::Vector`

The parsing fails and throws an exception if:

-   Required argument was not provided
-   Value was not provided for a named option
-   Type mismatch of provided value and the argument (e.g non-integer like string to an integer value)
-   Unknown/unmatched argument
-   Syntax error (usually interpreted as missing value or value of incorrect type)
-   Incomplete option definition (e.g. missing bound value)

## Known Issues

### Affects Users

-   None

### Does Not Affect Users

-   [**MSVC 16.9.3**] Using `type_info` requires `using ::type_info;` and include of `<typeinfo>` in the global module fragment, otherwise the `type_info` struct is unknown at compile time. Reported bug: https://developercommunity.visualstudio.com/t/Undefined-reference-to-type_info-when-us/1384072
-   [**clang 11.1.0**] Clang does not yet implement modules partitions. As a workaround each partition is guarded with `ifdef` removing the module partition stuff from the top and on clang they are all textually included (with `#include`) into the module interface directly. Once clang does implement module partitions this mechanism can be removed.
