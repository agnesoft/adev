# High Level Design

-   [Problem](#problem)
-   [Requirements](#requirements)
-   [Existing Solutions](#existing-solutions)
-   [ATest](#atest)
    -   [test()](#test)
    -   [expect()](#expect)
    -   [suite()](#suite)
    -   [Matchers](#matchers)
    -   [Printers](#printer)
    -   [Reporters](#reporter)
    -   [Usage](#usage)

## Problem

"If you liked it, then you should have put a test on it." -Beyoncé rule

Test is a software written to validate assumptions about another software and is essential for both development and maintenance. Testing frameworks aim to facilitate writing tests by providing common functionality such as bootstrapping, validation or reporting.

## Requirements

A modern C++ testing framework should:

-   Be simple to write tests in.
-   Be extensible.
-   Be a C++20 module.
-   Not use macros.
-   Provide good reporting of failures.

## Existing Solutions

There are many C++ testing frameworks written over the years. The most popular is [Google Test](https://github.com/google/googletest) (a.k.a. GTest). GTest is a heavy framework with lots of features (complemented by Google Mock). It feels clunky and the macros and the syntax is a bit strange (e.g. `TEST_F`, `ASSERT_EQ`). It also uses rather obsolete concept of `Fixtures` with a single pair of `SetUp` and `TearDown` methods making the testing quite inflexible and does not provide freeform test descriptions. Another more recent addition is [Catch2](https://github.com/catchorg/Catch2) that is also macro based. It feels much more natural and provides lots of flexibility and nice features including extensibility mechanisms. It is however single header library with significant compile time overhead and is currently being rewritten into more modularized form. One of the most recent additions is a [μt](https://github.com/boost-ext/ut) that is specifically designed with the above requirements in mind. Unfortunately as a prototype it does not really work. Its modules version cannot even be compiled and some of the choices, pecifically string literal (ab)use, make the tests somewhat weird.

When perusing other languages and their testing frameworks one in particular stands out - `jest` from JavaScript. It composes its tests and suites using `describe` and `test` that can be freely nested to create hierarchies of tests. More importantly however it composes its assertions using the [builder pattern](https://en.wikipedia.org/wiki/Builder_pattern). That helps readability of tests and makes understanding of what went wrong much easier than plain code wrapped in macros of string literals. Consider:

```
sum(1, 1) == 3_i; //μt
ASSERT_EQ(sum(1,1,), 3); //GTest
EXPECT_THROW(sum(1, 1), std::exception); //Catch2
```

vs

```
expect(sum(1, 1)).toBe(3); //jest
expect(() => { sum(1, 1) }).toThrow("exception text"); //jest
```

Particularly with more complex expressions having the textual aid of what is being asserted is simply superior to trying to dechiper plain code and guessing what is the test framework and what is the code being tested.

## ATest

The `ATest` is supposed to be lightweight minimalist C++20 module testing library without macros and with builder pattern assertings.

Example demonstrating the intent:

```
import atest;

auto sum(int x, int y) -> int
{
    return x + y;
}

auto main(int argc, char *argv[]) -> int
{
    using namespace atest;

    test("Some test", [] {
        expect(sum(1, 2)).toBe(3);
        expect([] { return sum(1, 2); }).toBe(3);
        expect([] { return sum(1, 2); }).toThrow<std::logic_error>("exception message");
    });
}
```

Possible output:

```
Running "Some test"... FAILED [150ms]
  at main.cpp:14:
    Exception text mismatch.
      Expected: "exception message"
      Actual  : "different message"
```

The `ATest` would get imported as a module that would make all the testing facilities available to the user within the namspace atest. While `using namespace` should generally be avoided it could help with brevity of the testing. The test is declared using a function `test` that takes a freeform description and a lambda (or any callable) containing the test code. Optionally the tests can be grouped in a `suite` that will act as a test collection. Suites are useful to give tests hierarchy or if there are multiple different kinds of tests in a single executable. Within the test code the assertions are always composed starting with `expect`. There are many different assertings one can compose. Validation is done by matchers. The run of the tests is measured and recorded and output using a `Printer` and a `Reporter`. By default, `Printer` will output to standard output and there is no `Reporter`. The printer and the reporter can be selected from the command line.

### test()

### expect()

### suite()

### Matchers

### Printers

### Reporters

### Usage
