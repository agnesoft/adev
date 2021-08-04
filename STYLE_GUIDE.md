# Style Guide

- [General Rules](#general-rules)
  - [Project & File Names](#project--file-names)
  - [Issue Names, Pull Request Names & Commit Messages](#issue-names-pull-request-names--commit-messages)
- [C++](#c)
  - [Formatting](#formatting)
  - [C++ Version](#c-version)
  - [Naming](#naming)
  - [Headers](#headers)
  - [Sources](#sources)
  - [Namespaces](#namespaces)
  - [Enumerators](#enumerators)
  - [Comments](#comments)
  - [Documentation](#documentation)
  - [Globals](#globals)
  - [Statics](#statics)
  - [Functions](#functions)
  

## General Rules

### Project & File Names

- only lower case alphanumeric characters (`a-z0-9`)
- no spaces (underscore `_` as separator if needed, no hyphen `-`)
- name must not start with a number

**Why?**

To avoid bugs due to Windows being case insensitive filesystem. (e.g. `App` can be called as `app` on Windows but not on Linux) and to avoid the need for quotation of the path/executable (e.g. `My program.exe` cannot be called without quotation). Furthermore code identifiers cannot start with numbers or contain hyphens so to achieve consistency between the files, binaries, project names and code the names are restricted to valid identifiers (e.g. `7-zip` project cannot contain a namespace or anything called `7-zip` in the code and would need to use `s_zip` or `szip` or `sevenzip` etc.). Limiting the names to basically ASCII also helps with portability and accessibility.

### Issue Names, Branch Names, Pull Request Names & Commit Messages

- `[project]` prefix to identify the work (use multiple if the work affects multiple, e.g. `[project1][proeject2]`
- always include issue number in PR, branch name and commit message, e.g. `#111`
- always start with a noun such as `Add`, `Remove`, `Update`, `Refactor`
- always match all four to the same text: `issue`, `branch` `PR`, `merge commit message`

---
**NOTE**

Git does not allow branches starting with `[` so remove the square brackets from the branch name. GitHub also loves reformatting and rewriting the names so be watchful of what it puts in the title when creating PR and **especially during merging**. For single commit PRs GitHub will suggest the one commit's message instead of the PR title as the merge commit message.

---

**Examples**
```
issue: [devops] Add style guide
branch: devops-Add-style-guide-#111
PR: [devops] Add style guide #111
commit: [devops] Add style guide #111 (#112)
```

**Why?**

The prefix in brackets identifers at a glance what project or component the given issue/PR/commit is concerned with. Noun at a start gives notion of the nature of the change and it the name is the same through the lifecycle of the change, from issue to branch via PR to the commit, it is much easier to keep track of and to find it later. The number in PR and commit also ensures that there is not a rogue change without any sort of tracking.

## C++

**Ground rules**

- Optimize for the reader of the code: the code will be read many more times than it is written.
- Don't be clever: any code that contains clever or peculiar tricks will be surprising to the reader which, in all likelihood, will be you two months from now.

### Formatting

Use `clang-format`.

### C++ Version

[C++20](https://en.cppreference.com/w/cpp/20).

### Naming

- *namespaces*: snake_case (e.g. `namespace ns`, `namespace my_ns`)
- *types*: CamelCase (e.g. `class MyClass`, `struct Iterator`, `typename T`)
- *enums*: CamelCase (e.g. `enum class MyEnum { EnumValue };`) 
- *macros*: ALL_CAPS (e.g. `#define MY_MACRO 3`)
- *functions*: mixedCase (e.g. `foo()`, `myBar()`)
- *variables*: mixedCase (e.g. `int myVar`, `std::string someDoc`)
  - *class members*: mixedCase_ (e.g. `int member_`, `char someVar_`)
  - *struct members*: mixedCase (e.g. `int interval`, `int orderId`)
  - *constants*: ALL_CAPS (e.g. `int MY_CONSTANT = 3`)

### Headers

**Do not use header files.** They are legacy way of sharing code between translation units. Use [C++20 modules](https://en.cppreference.com/w/cpp/language/modules) instead.

If you must use a header file:

- include with `""` only headers that can be located by the relative path from the current file (i.e. files in the same directory)
- include with `<>` headers from all other sources (other projects, system headers etc.)
- disambiguate the include if possible with a path prefix (e.g. `projectX/header.h` instead of `header.h`)
- use `clang-format` for ordering

If you must use a header file:

- use `*.hpp` extension to distinguish the header from C header (C++ headers could rarely be compiled in C)
- use preprocessor include guard `ifndef/define/endif` rather than non-standard `pragma once`
- the include guard define should have format `PROJECT_HEADER_HPP` (e.g. `atest/expect.hpp `ATEST_EXPECT_HPP`)
- prefer forward declarations over includes for symbols that are only used internally in the source file
- prefer includes over forward declarations for symbols used in public API (e.g. `#include <string> ... void foo(const std::string &str);` so that the user of the header does not need to guess what else is needed to use it

### Sources

Source files should follow the general rule for [naming files](#project--file-names):

- use `*.cpp` extension for regular sources
- use `*.ixx` for module interfaces
- name the file according to its contents, preferably after the class/module/partition it defines (e.g. `class MyClass` -> `myclass.cpp`)

### Namespaces

- place all code in a namespace
- use only single level of namespaces
- the namespace name should be the project name (e.g. project `atest` -> `namespace atest`)
- never use `using namespace` directive (especially `using namespace std;`)

### Enumerators

- use `enum class`
- do not use old `enum`

### Comments

- do not use comments unless you are documenting an entity
- refactor and create a named functions instead of commenting
- if you must use a comment prefer `//` over `/* */`

### Documentation

- always document externally visible public and protected symbols (i.e. exported or header declared classes, functions, structs, enums etc.)
- use Doxygen style `//!` comments for in-code documentation in front of the symbol (i.e. function, class, member variable)
- never document private code
- add brief summary when documenting classes (e.g. `\brief The X provides something`)
- focus on `what` a documented entity does rather than on `how` or `why` it does it
- always include examples of usage

### Globals

- avoid using global variables
- never use mutable global variables
- only use const global variables that cannot throw during initialization (e.g. a type is trivial)
- always place them in a namespace

**Why?**

There are many issues with global variables. They are initialized before `main()` and therefore any initialization error during the construction cannot be handled. Furthermore mutable global variables will obscure application flow and make it very hard to follow the program flow as globals are accessible from anywhere. The only justifiable use case is for global constants.

### Statics

- avoid using global static variables including as class members
- never use mutable global static variables
- only use global static variables that are const, cannot throw during initialization (e.g. a type is trivial) and that do not use each other
- prefer static variables in methods that ensure the initialization on first use

**Why?**

Unlike with global variables the order of initialization of static variables is undefined. Using one to initialize another might lead to [initialization order fiasco](https://en.cppreference.com/w/cpp/language/siof). Furthermore static variables have internal linkage that can lead to surprising behavior when used with dynamic libraries where each side has its own copy of the seemingly same static variable.

### Functions
