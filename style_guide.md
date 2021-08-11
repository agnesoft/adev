# Style Guide

-   [General Rules](#general-rules)
    -   [Ground Rules](#ground-rules)
    -   [Project & File Names](#project--file-names)
    -   [Issue Titles, Branch Names, Pull Request Titles & Commit Messages](#issue-titles-branch-names-pull-request-titles--merge-commit-messages)
    -   [Project Structure](#project-structure)
    -   [Project Design](#project-design)
    -   [Project Readme](#project-readme)
-   [C++](#c)
    -   [Version](#version)
    -   [Language Extensions](#language-extensions)
    -   [Formatting](#formatting)
    -   [Naming](#naming)
    -   [Headers](#headers)
    -   [Sources](#sources)
    -   [Modules](#modules)
    -   [Scoping](#scoping)
    -   [Includes](#includes)
    -   [Imports](#imports)
    -   [Preprocessor](#preprocessor)
    -   [Namespaces](#namespaces)
    -   [Classes](#classes)
    -   [Structs](#structs)
    -   [Enumerators](#enumerators)
    -   [Initialization](#initialization)
    -   [Memory Management](#memory-management)
    -   [Multithreading](#multithreading)
    -   [Aliases](#aliases)
    -   [Functions](#functions)
    -   [Lambdas](#lambdas)
    -   [Templates & Concepts](#templates--concepts)
    -   [Exceptions](#exceptions)
    -   [Run Time Type Information (RTTI)](#run-time-type-information-rtti)
    -   [Local Variables](#local-variables)
    -   [Global Variables](#global-variables)
    -   [Static Variables](#static-variables)
    -   [Preincrement vs Postincrement](#preincrement-vs-postincrement)
    -   [Integer Types](#integer-types)
    -   [Casts](#casts)
    -   [Literals](#literals)
    -   [Ternary](#ternary)
    -   [this](#this)
    -   [const](#const)
    -   [constexpr](#constexpr)
    -   [consteval](#consteval)
    -   [virtual](#virtual)
    -   [inline](#inline)
    -   [noexcept](#noexcept)
    -   [nullptr](#nullptr)
    -   [sizeof](#sizeof)
    -   [auto](#auto)
    -   [friend](#friend)
    -   [Comments](#comments)
    -   [Documentation](#documentation)

## General Rules

### Ground rules

-   optimize for the reader of the code: the code will be read many more times than it is written
-   don't be clever: any code that contains clever or peculiar tricks will be surprising to the reader which, in all likelihood, will be you two months from now
-   what is not covered by the style guide is generally fine to use/do
-   if in doubt: ask

### Project & File Names

-   snake_case
    -   use only lower case alphanumeric characters (`a-z0-9`)
    -   spaces are replaced with underscore (`_`)
    -   name must not start with a number

### Issue Titles, Branch Names, Pull Request Titles & Merge Commit Messages

-   always use `[project]` prefix to identify the work (use multiple if the work affects multiple, e.g. `[project1][project2]`)
-   always include issue number in branch name, PR title and commit message, e.g. `#111`, `(#111)`
-   always start with a noun such as `Add`, `Remove`, `Update`, `Refactor` (capitalization is useful to distinguish it from the project affiliation)
-   always match all four: issue title, branch name, pull request title & merge commit message

**Examples**

_issue:_ `[devops] Add style guide`

_branch:_ `devops-Add-style-guide-#111`

_PR:_ `[devops] Add style guide #111`

_merge commit:_ `[devops] Add style guide #111 (#112)`

---

**NOTE 1**

Git does not allow branches starting with `[` so remove the square brackets from the branch name.

**NOTE 2**

GitHub reformats issue titles, PR titles and commit messages (especially when there is only one commit in a branch). Double check these before saving or committing.

### Project Structure

-   all projects are located under `projects/` directory
-   sources should be placed directly in the project directory
-   public headers ([if headers are used](#headers)) should be placed in `include/<project name>` directory within the project directory so that they can be included without ambiguity as `<project name>/<header>.hpp`
-   private headers should be placed with the source files
-   every project should have a `test/` directory with a test application
-   projects can have sub-projects (they should be rare) placed directly in the parent project's directory (e.g. `projects/abuild/preprocessor`)
-   every project must have a `readme.md`
-   every project must have a `design.md` (and should start with it)

### Project Design

-   use markdown
-   always provide table of contents
-   the purpose of the design is to provide guidance during implementation and when the project code is being investigated in the future
-   every design must have 4 sections:
    -   **problem**: what is the project trying to solve?
    -   **requirements**: what are the features the project must have to solve the problem?
    -   **existing solutions**: why the existing solutions do not meet the requirements?
    -   **design**: how do we solve the problem and meet the requirements?
-   design should focus on high level overview - overall architecture, description of flows, components, usage etc.
-   keep it alive - design does not need to be perfect, it will diverge as the implementation evolves
    -   **always update or delete sections that are not correct or relevant**

### Project Readme

-   use markdown
-   always provide table of contents
-   the purpose of the readme is to provide quick overview of the project
    -   what the project does
    -   how is the project used
-   every readme must have 3 sections:
    -   **overview**: what the project does/provides and what are its features
    -   **prerequisites**: what are the prerequisites & dependencies of using the project
    -   **usage**: steps to start using the project with examples, e.g.
        1. add build dependency to `myproject`
        2. add `import myproject;` in your code
        3. call `myproject::foo()` to do X
        4. call `myproject::bar()` to do Y
-   add additional sections and information as you deem fit such as `Known Issues`

---

## C++

### Version

-   [C++20](https://en.cppreference.com/w/cpp/20)

### Language Extensions

-   do not use language extensions, no exception

### Formatting

-   use `clang-format`

### Naming

-   _modules_: dot.case (e.g. `my.module`, `adev.commandline`)
-   _namespaces_: snake_case (e.g. `namespace ns`, `namespace my_ns`)
-   _types_: CamelCase (e.g. `class MyClass`, `struct Iterator`, `typename T`)
-   _enums_: CamelCase (e.g. `enum class MyEnum { EnumValue };`)
-   _macros_: SCREAMING_SNAKE_CASE (e.g. `#define MY_MACRO 3`)
-   _functions_: mixedCase (e.g. `foo()`, `myBar()`)
-   _variables_: snake_case (e.g. `int my_var`, `std::string some_doc`)
-   _constants_: SCREAMING_SNAKE_CASE (e.g. `int MY_CONSTANT = 3`)

---

**NOTE**

Use regular `snake_case` for class member variables as well and use `this->` to disambiguate them from other variables.

---

### Headers

-   do not use or create header files, use [imports](#imports) and [modules](#modules) instead

If you must create a header file:

-   use `*.hpp` extension to distinguish the header from C header (C++ headers are rarely compilable in C)
-   use preprocessor include guard `#ifndef/#define/#endif` rather than non-standard `pragma once`
-   the include guard define should have format `PROJECT_HEADER_HPP` (e.g. `atest/expect.hpp` -> `ATEST_EXPECT_HPP`)
-   never forward declare symbols used in public API (e.g. `#include <string> ... void foo(const std::string &str);`) so that the user of the header does not need to guess what else is needed to use the header
-   forward declare symbols used internally and move the includes into the source file

### Sources

-   use `*.cpp` extension for source files
-   name the files according to their content, ideally after the class/module/partition they define (e.g. `class MyClass` -> `myclass.cpp`, `export module atest` -> `atest.ixx`)

### Modules

-   the module name should be the project name (e.g. `atest` -> `export module atest`)
-   only one module interface per project
-   avoid the use of global module fragment and includes in general, import them as header units instead
-   split modules into interface and module partitions
-   define each module partition in its own file
-   keep module partitions small, i.e. one class per partition
-   export only what is part of the public API, you do not have to export transitively unless the transitive symbol is part of the API as well (as opposed to be just used by the API), e.g. `class C; export C foo();` is fine because C cannot be instantiated outside of the module and can be used only via the exported API, i.e. function `foo`
-   avoid exporting entire namespaces

### Scoping

-   always use `{}` even for statements with single statement (e.g. `if (true) { foo(); }`)
-   avoid use of arbitrary scoping (`e.g. void foo() { { fizz(); } buzz(); }`), make a function or lambda instead

### Includes

-   do not use `#include`, use [import](#imports) instead

If you must use an include:

-   include with `""` only headers that can be located by the relative path from the current file, i.e. files in the same directory
-   include with `<>` headers from all other sources (other projects, system headers etc.)
-   disambiguate the include if possible with a path prefix (e.g. `projectX/header.h` instead of just `header.h`)
-   use `clang-format` for ordering

### Imports

-   import with `""` only header units that can be located by the relative path from the current file, i.e. files in the same directory
-   import with `<>` header units from all other sources (other projects, system headers etc.)
-   disambiguate the imports of header units if possible with a path prefix (e.g. `projectX/header.h` instead of just `header.h`)

### Preprocessor

-   never create macros
-   never create preprocessor defines
-   never use macros unless they are unavoidable part of third party API
-   avoid the use of preprocessor defines unless it is needed for platform specific code (e.g. `ifdef _WIN32`)

### Namespaces

-   place all code in a namespace
-   use only single level of namespaces
-   the namespace name should be the project name (e.g. project `atest` -> `namespace atest`)
-   never use `using namespace` directive (especially `using namespace std;`)

### Classes

-   do the initialization in constructor (no two phase initialization) and throw an [exception](#exceptions) in case of an error
-   single parameter constructors must be declared `explicit`
-   no implicit conversions, mark conversion operators `explicit`
-   always declare member variables private and use accessor methods
-   be mindful of padding when ordering member variables
-   follow the rule of 0 or rule of 5
-   avoid multiple inheritance
-   prefer composition to inheritance
-   never inherit virtually

### Structs

-   no member functions
-   be mindful of padding when ordering member variables
-   prefer custom structs over `std::pair` and `std::tuple`

### Enumerators

-   use `enum class`
-   do not use old `enum`

### Initialization

-   use `=` for scalar types (e.g. `int i = 10;`)
-   use aggregate initialization `{}` for all other types (e.g. `MyClass my_class{"hello"};`)
-   use designated initializers for structs (e.g. `S s{.member1 = 1, .member2 = "value"};`)
-   never use `()` for initialization unless necessary (e.g. certain `std::string` constructors)

### Memory Management

-   prefer stack over heap
-   use smart pointers and prefer `std::unique_ptr`
-   avoid use of `std::shared_ptr` unless it is necessary
-   never manage memory manually, i.e. use naked `new` and `delete`
-   never manage raw arrays on the heap (`new[]`, `delete[]`), use `std::array` instead
-   never use `malloc/free` unless a third party API requires it

### Multithreading

-   avoid multithreading if possible
-   avoid using locks if possible, consider using [coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
-   prefer highest possible level of abstraction (e.g. atomics, coroutines) to low level multithreading primitives (e.g. mutex)

### Aliases

-   never use `typedef`, use `using` instead
-   avoid type aliasing: do not use type aliases for regular types (e.g. `using MyList = std::vector<int>`), always spell out the type for clarity
-   use type aliasing for complicated template types such as custom iterators (e.g. `using iterator = MyTemplatedIterator<ValueType, ConstValueType, SomeFlag>`)
-   use type aliasing to satisfy type traits, e.g. `using value_type = ...`

### Functions

-   ~ 4 lines of code per function (split your code into more well named functions)
-   prefer 1 to 2 arguments, max 4 individual arguments
-   use custom `struct` to pass in more than 4 arguments
-   do not use default arguments, use overloads instead
-   prefer return values over output arguments

### Lambdas

-   avoid lambdas that escape current scope
-   avoid captures if possible
-   prefer `&` capture for local lambdas
-   prefer `=` capture for lambdas that escape current scope
-   do not use capture list solely to rename variables

### Templates & Concepts

-   use `concepts` (`requires`) instead
-   never use SFINAE, use `concepts` instead

### Exceptions

-   use exceptions for error handling
-   always inherit from `std::exception`

### Run Time Type Information (RTTI)

-   always enabled

### Local Variables

-   declare variables close to their first use
-   declare variables with narrowest scope possible
-   declare varitables [const](#const) or [constexpr](#constexpr) correct
-   consider using conditional and loop initializers for variables bound only to that scope (e.g. `if (int i = foo(); i == 1) {}`, `while (int i = 0; i < 10) { ++i; }`

### Global Variables

-   avoid using global variables
-   never use mutable global variables
-   only use const global variables that cannot throw during initialization (e.g. a type is trivial)
-   always place them in a namespace

### Static Variables

-   avoid using global static variables including as class members
-   never use mutable global static variables
-   only use global static variables that are const and that cannot throw during initialization (e.g. a type is trivial) and that do not use other static variables in their initialization
-   prefer static variables in functions that ensure the initialization on first use

### Preincrement vs Postincrement

-   prefer preincrement

### Integer Types

-   use fixed length integer types in public APIs
-   use `int` only in internal code
-   prefer signed integer types
-   avoid unsigned integer types in loops
-   avoid comparing signed and unsigned integers and avoid casting in such cases, use `cmp_*` family of STL functions instead

### Casts

-   never use C-cast, i.e. `(int)var`
-   never use `const_cast`
-   use `reinterpret_cast` only if you know your use case is not [undefined behavior](https://en.cppreference.com/w/cpp/language/reinterpret_cast)

### Literals

-   do not use user defined literals
-   use digit separators for long values (e.g. `1'000'000`)
-   prefer specifying the type via a literal in case of possible ambiguity (e.g. `auto v = 5.0f`)

### Ternary

-   avoid ternary operator, prefer full `if/else` instead
-   use ternary for simplest of cases such as one line functions with very clear logic

### `this`

-   always use `this->` when calling member functions (methods)
-   always use `this->` when accessing member variables

### `const`

-   always be const correct
-   use west const

### `constexpr`

-   use as much as possible

### `consteval`

-   always use for purely compile time calculations to avoid leaking them to runtime via `constexpr`

### `virtual`

-   avoid runtime polymorphism
-   use compile time alternatives such as [CRTP](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
-   always use `override`
-   never put `virtual` and `override` on the same function
-   never change accessor of the function you are overriding (e.g. `private` -> `public`)

### `inline`

-   do not use unless it is required (e.g. standalone function definitions in a header)

### `noexcept`

-   use when the entire call chain of the function is noexcept
-   avoid use of `noexcept` constraints

### `nullptr`

-   always use `nullptr`
-   never use `NULL`
-   never use `0` as a pointer value

### `sizeof`

-   prefer the variable as argument (not the type)

### `auto`

-   almost never auto
-   use auto when necessary such as with lambdas (e.g. `auto l = [] {};`)
-   use auto when the assignment expression denotes explicitly the type already (e.g. `auto i = static_cast<int>(var);`)
-   use auto when the explicit type would obfuscate the code such as when using iterators in loops (e.g. `for (auto it = v.begin(); it != v.end(); ++it) {}`)

### `friend`

-   avoid using friend classes
-   use friends only wheen necessary (e.g. standalone comparison operators)

### Comments

-   avoid commenting code unless you are [documenting](#documentation) it
-   refactor and create a named functions instead of commenting
-   if you must use a comment prefer `//` over `/* */`

### Documentation

-   always document externally visible public and protected symbols (i.e. exported or header declared classes, methods, functions, structs, member variables, enums etc.)
-   use Doxygen style `//!` comments for in-code documentation in front of the symbol (i.e. function, class, member variable)
-   never document private code
-   add brief summary when documenting classes (e.g. `\brief The X provides something`)
-   focus on `what` a documented entity does rather than on `how` or `why` it does it
-   always mention assumptions (e.g. about arguments), side effects and results (e.g. return value)
-   include examples of usage
