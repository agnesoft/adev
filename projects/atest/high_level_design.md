# High Level Design

-   [Problem](#problem)
-   [Requirements](#requirements)
-   [Existing Solutions](#existing-solutions)
-   [ATest](#atest)
    -   [test()](#test)
    -   [expect()](#expect)
    -   [suite()](#suite)
    -   [Matchers](#matchers)
    -   [Printers](#printers)
    -   [Reporters](#reporters)
    -   [Usage](#usage)

## Problem

"If you liked it, then you should have put a test on it." -Beyoncé rule

Test is a software written to validate assumptions about another software and is essential for both development and maintenance. Testing frameworks aim to facilitate writing tests by providing common functionality such as bootstrapping, validation or reporting.

## Requirements

A modern C++ testing framework should:

-   Be simple to write tests in.
-   Be a C++20 module.
-   Not use macros.
-   Provide good reporting of failures.
-   Be extensible.
-   Be transparent.

## Existing Solutions

There are many C++ testing frameworks written over the years. The most popular is [Google Test](https://github.com/google/googletest) (a.k.a. GTest). GTest is a heavy framework with lots of features (complemented by Google Mock) that however feels clunky and out of date. The macros and the syntax is a bit strange (e.g. `TEST_F`, `ASSERT_EQ`). It also uses rather obsolete concept of `Fixtures` with a single pair of `SetUp` and `TearDown` methods making the testing quite inflexible. Furthermore it does not even provide free form test descriptions. Another more recent addition is [Catch2](https://github.com/catchorg/Catch2) that is also macro based. It feels much more natural and provides lots of flexibility and nice features including extensibility mechanisms. It is however single header library with significant compile time overhead and is currently being rewritten into more modularized form. One of the most recent additions is a [μt](https://github.com/boost-ext/ut) that is specifically designed with the above requirements in mind. Unfortunately as a prototype it does not really work. Its modules version cannot even be compiled and some choices, specifically string literal (ab)use, make the tests somewhat weird.

When perusing other languages and their testing frameworks one in particular stands out - `jest` from JavaScript. It composes its tests using `describe` and `test` that can be freely nested to create hierarchies of tests. More importantly however it composes its assertions using the [builder pattern](https://en.wikipedia.org/wiki/Builder_pattern). That helps readability of tests and makes understanding of what went wrong much easier than plain code wrapped in macros of string literals. Consider:

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

Particularly with more complex expressions having the textual aid of what is being asserted (tested) is simply superior to trying to decipher plain code and guessing what is the test framework and what is the code being tested. Test descriptions or test sections including BDD style given/when/then can come only so far. It also allows arbitrary extension to the assertions such as `toMatchObject` which is an assertion that will be true as long as the expectation is the subset of an actual object. A custom matcher that would be used behind the scenes would be traditionally required for the same with assert macros. Debugging such a failed assertion would therefore be... surprising. Transparency in what is going on is essential in aiding the developers using a testing framework. They usually care very little for fancy printing or feature richness, but they care a lot for what the experience is like when a test fails.

## ATest

The `ATest` is supposed to be lightweight, minimalistic C++20 module testing library without macros and with extensible builder pattern asserts.

Example:

```
// Import it as C++20 module
import atest;

// Code to be tested
auto sum(int x, int y) -> int
{
  return x + y;
}

// Tests are added in main
auto main(int argc, char *argv[]) -> int
{
  // Import testing stuff into local namespace for convenience
  using namespace atest;

  // Initialize ATest [OPTIONAL]
  atest(argc, argv);

  // Adds the tests into a named suite [OPTIONAL]
  suite("My Test Suite", [] {

    // Starts a named test
    test("Some test", [] {
    // Simple assertion
    expect(sum(1, 2)).toBe(3);

    // Assertion with delayed call to get better exception diagnostics
    expect([] { return sum(1, 2); }).toBe(3);

    // Assertin of a thrown exception
    expect([] { return sum(1, 2); }).toThrow<std::logic_error>("exception message");
    });
  })
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

### suite()

The `suite(name, fn)` is an optional wrapper function that operates similar to jest's [describe()](https://jestjs.io/docs/api#describename-fn) and is used for grouping tests. The tests are declared inside the function (i.e. lambda) passed in.

### test()

The `test(name, fn)` is the core building block in `atest` and declares a test. The test body is passed in as a function that takes no arguments and returns nothing (i.e. lambda). The test can be declared inside a `suite`. Tests will be run in the order as declared. The tests are time measured.

### expect()

Each assertion begins with a call to the `expect` function that takes any expression. The `expect` returns an object that lets you perform actual assertions using various functions such as `toBe`, `toThrow` etc. The `expect` also captures the call site location for reporting purposes. Various functions can be called on the returned object that lets you finalize the assertion with the expected value, optionally passing in a custom matcher.

### Matchers

The assertion functions imply what matcher to use when comparing the actual value with the expectation. For example `toBe` implies use of `operator==`. There is a generic `toMatch` function that requires a custom matcher to be used as template argument.

### Printers

By default, the output is written to standard output using `std::cout`. A different printer supporting streaming operator `operator<<` can be used and installed. Each value to be printed is first printed using `std::stringstream::operator<<`. In order to support your own printing implement the `std::stringstream::operator<<` for your type.

### Reporters

By default, no reports are provided. A reporter can be specified on the command line (e.g. `junit`). Additional reporters can be installed programmatically. The reports output the test results in a specified format.

### Usage

By default, simply building and running the test program should be enough. Optionally command line parameters can be supplied and then the `atest` must be initialized with them (i.e. `atest::atest(argc, argv)`). The command line parameters should provide:

-   Ability to filter out `tests` and `suites`
-   Ability to execute only set of `tests` and `suites`
-   Choose a Printer
-   Choose a Reporter
