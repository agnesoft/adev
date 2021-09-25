# acommandline

-   [Overview](#overview)
-   [Prerequisites](#prerequisites)
-   [Usage](#usage)

# Overview

Strongly typed command line parser that takes standard `main()` function arguments `int` and `*char[]` and parses them into user variables according to options defined by the user with the builder pattern.

# Prerequisites

-   C++ compiler
-   C++20
-   Exceptions
-   [astl](projects/astl/readme.md)

# Usage

At a glance:

```
import acommandline;

auto main(int argc, char *argv[]) -> void
{
    std::string longValue;
    bool flag = false;
    std::vector<int> numbers;

    ::acommandline::CommandLine parser;
    parser.option().long_name("longName").description("Some option").bind_to(&longValue);
    parser.option().long_name("flag").short_name('f').default_value(true).description("Some flag").bind_to(&flag);
    parser.option().positional().description("Input numbers").bind_to(&numbers);
    parser.parse(argc, argv);

    //Possibly called as: ./myapp -f --longName "my value" 1 2 3
    //longValue == "my value"
    //flag == true
    //numbers == { 1, 2, 3 }
}
```

Each option is defined using the builder pattern in nearly natural English. As a final step the option is bound to an existing variable where the value will be stored during parsing. The parser is capable of some limited type conversions but generally enforces strict typing. It supports following types:

-   bool
-   std::string
-   std::int64_t
-   double
-   std::vector\<std::string\>
-   std::vector\<std::int64_t\>
-   std::vector\<double\>

When the `std::vector` type is bound the option is automatically considered repeatable (can be matched multiple times). An option can be optional defined as `required()` or with `default_value()`. Unmatched required option is a parsing error. Unmatched defaulted option will save the default value to the bound variable.

Options can be either named or positional. Named options will only match the long name when starting with two dashes (`--`) or short name if it was specified starting with one dash (`-`). The short name can be only a single uppercase or lowercase letter. The long name can be any combination of alphanumeric characters plus `_`, `-` and `.` but it always must start with a letter. Positional options can be matched anywhere provided they can be stored or converted to the bound variable.

The `bool` variable indicates a flag and cannot have a value. Named variables can specify a value with either a space or `=`. Values can be quoted unless they are a number.

Examples of valid values:

```
--flag
--longname=my_value
--long-name="my value"
--my_number=12345
--Long.Name my_value
--long-name "my value"
--my_number 15.50
-F
-s=my_value
-s="my value"
-n=12345
-N=15.50
-s my_value
-S "my value"
-n 12345
-N 15.50
positional_value
"positional value"
12345
15.50
```

Providing description for each option is required and the parser generates automatic help that can be displayed with `-?` argument. The parser will throw an exception if there is an error during parsing such as missing required argument, type mismatch etc.
