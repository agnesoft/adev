# atest

-   [Overview](#overview)
-   [Prerequisites](#prerequisites)
-   [Usage](#usage)
    -   [Using Declarations](#using-declarations)
    -   [test()](#test)
    -   [suite()](#suite)
    -   [expect(), assert()](#expect-assert_)
        -   [to_be()](#to_be)
        -   [to_match()](#to_match)
        -   [to_throw()](#to_throw)
        -   [expect_fail(), assert_fail()](#expect_fail-assert_fail)
    -   [Test Runner](#test-runner)
    -   [Filtering & Listing](#filtering--listing)
    -   [Printer](#printer)
-   [Known Issues](#known-issues)

# Overview

The `atest` module is a C++20 testing framework. It is completely macro free and is inspired by [Jest](https://jestjs.io/) (JavaScript testing framework) utilizing builder pattern and test (suite) composing.

# Prerequisites

-   C++ compiler
-   C++20:
    -   `source_location`
    -   `concepts`
-   RTTI
-   Exceptions
-   [astl](/projects/astl/readme.md)

# Usage

At a glance:

```
import atest;

static const auto S = ::atest::suite("My test suite", [] {
    test("a test", [] {
        ::atest::expect(1 + 1).to_be(2);
    });
});

auto main(int argc, char *argv[]) -> int
{
    test("global test", [] {
        ::atest::expect([] { throw std::logic_error{"error"}; }).to_throw<std::logic_error>("error");
    });

    return atest::TestRunner{argc, argv}.run();
}
```

## Using Declarations

It is recommended to use the following set of using declarations to make the tests easier to write & read:

```
using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;
```

## test()

```
auto atest::test(const char *name, auto (*body)()->void) -> void
```

Registers a test `body` under the name `name`. It can be called either within a [test suite](#suite) or in the `main()`. When not called within a `suite()` call the test will be registered in the implicit `global` (nameless) test suite. The second argument is a nullary function that takes no arguments and returns nothing. It would typically be a lambda.

Example:

```
import atest;
int main()
{
    ::atest::test("My test", [] {
        //...
    });
}
```

## suite()

```
auto atest::suite(const char *name, auto (*body)()->void) noexcept -> int
```

Groups tests into a test suite called `name`. It can be called in `main()` or outside of main capturing the result in a global/static variable in order to let the suite and tests be registered. The function will return `0` if it succeeds or `1` if an exception has been thrown during the registration. No exceptions are propagated as the function is typically called before `main()` and thus it is not possible to capture and handle the exceptions within such context.

Example:

```
import atest;
static auto s= ::atest::suite("My test suite", [] {
    ::atest::test("My test", [] {
        //...
    });
});
```

## expect(), assert\_()

```
template<typename T>
auto atest::expect(const T &value) noexcept -> Expect<T>

template<typename T>
auto atest::assert_(const T &value) noexcept -> Expect<T>
```

Starts composition of the test's expectation or assertion. The difference is that `assert_` will stop the test when it fails while `expect` will continue even if it fails. Any `T` is allowed as argument. If `T` is a callable it will be executed once the assertion is finalized. No exceptions will be propagated from the call. The return value is the internal class `Expect` that lets you select from one of the supported expectations:

### to_be()

```
template<typename V>
auto to_be(const V &value) -> ExpectToMatch
```

Completes the expectation using the default matcher that uses `operator==` to match the values. Custom types can be supported by simply defining the `operator==` for them. Note that if your type will be nested in a container such as `std::vector` its `operator==` might need to be implemented as a member function rather than a standalone function for ADL (Argument Dependent Lookup) to find it.

Examples:

```
::atest::expect(1 + 1).to_be(2); //matching values
::atest::expect([] { return 1 + 1; }).toBe(2); //matching result of a callable and a value
::atest::assert_(1 + 1).toBe(2);
```

### to_match()

```
template<typename M, typename V>
auto to_match(const V &value) -> ExpectToMatch
```

Completes the expectation using a custom matcher. The custom matcher can be any callable type that takes parameters of type `T` and `V` to perform the matching (i.e. `auto operator()(const T&, const V&) -> bool`). Furthermore the matcher needs to have the following methods:

-   `auto describe() -> std::string`
-   `auto expected(const T &, const V &) -> std::string`
-   `auto actual(const T &, const V &) -> std::string`

These methods are used for reporting a failure in case it occurs. The first function gives description of the expectation or of the operation the custom matcher was performing. The other two lets you customize the output for the `expected` and `actual` outcomes of the match and will be given the actual values. You can inherit from `atest::MatcherBase` if any or all of these functions do not need to be specialized for your matcher. If you are implementing them yourself you may use the convenience `auto atest::stringify(const T &...values) -> std::string` to convert `T`, `V` or anything else to string (requires `auto operator<<(std::ostream &, const T &value) -> std::ostream &` to be implemented for the type). The `stringify` knows how to print containers so you only need the operator for the inner type.

Example:

Minimal matcher:

```
struct MyLessMatcher : ::atest::MatcherBase
{
    auto operator()(int left, int right) -> bool { return left < right; }
};
```

Full matcher:

```
struct MyLessMatcher //no need to inherit anything
{
    auto describe() -> std::string { return "Left value to be less than right value."; }
    auto expected(int left, int right) -> std::string { return std::to_string(left) + " < " + std::to_string(right) }
    auto actual(int left, int right) -> std::string { return std::to_string(left) + (left == right ? std::string{" = "} : std::string{" > "}) + std::to_string(right); }
    auto operator()(int left, int right) -> bool { return left < right; }
};

::atest::expect(1).toMatch<MyMatcher>(2);
```

### to_throw()

```
template<typename E>
auto to_throw() -> ExpectToThrow

template<typename E>
auto to_throw(const E &exception) -> ExpectToThrow

template<typename E, typename V>
auto to_throw(const V &value) -> ExpectToThrow
```

Completes the matching by expecting an exception. The `T` passed to the `expect()` must be a callable for this expectation. The exception type may be specified as a template argument to the call or deduced from the value. In either case strong type matching is performed using `typeid` (RTTI) and the exact match is required (i.e. expecting a base class but throwing a derived class is a failure). If a value was passed the exception will be additionally validated against it as well. If the exception type implements `what()` method returning a type that is convertible to `std::string` the result of `what()` will be compared against the value. If there is no `what()` method (e.g. an `int` or `std::string` is thrown) the value is compared directly to the exception object. The value comparison requires `operator==(const E &e, const V &v)` to compile. Additionally the exception type that does not have `what()` must be printable (the operator `auto operator<<(std::ostream &, const E &exception) -> std::ostream &` must exist).

Examples:

```
:;atest::expect([] { throw std::exception{}; }).to_throw<std::exception>();
:;atest::expect([] { throw std::logic_error{"Some text"}; }).to_throw<std::logic_error>("Some text");
:;atest::expect([] { throw std::logic_error{"Some text"}; }).to_throw(std::logic_error{"Some text"});
:;atest::expect([] { throw 1; }).to_throw<int>();
:;atest::expect([] { throw 1; }).to_throw(1);
```

### expect_fail(), assert_fail()

```
template<typename T>
auto atest::expect_fail(const T &value) noexcept -> Expect<T>

template<typename T>
auto atest::assert_fail(const T &value) noexcept -> Expect<T>
```

Modifies the expectation or assertion to reverse the result. If the expectation/assertion passes it will be converted into an error (and stop the test in case of `assert_fail`). Conversely if the expectation/assertion fails it will be considered a success. No additional output will be printed regarding the failure. It is primarily useful for testing negative scenarios. Note that `expect_fail` will still fail on unexpected exceptions except when reversing `to_throw()`.

Example:

```
::atest::expect_fail(1).toBe(2); //passes despite failing
::atest::assert_fail([] {}).toThrow<int>(); //passes despite failing
```

## Test Runner

The test runner is the main entry point of `atest`. The registrations of the test suites and tests are global and the user is responsible to actually start the test run by instantiating `atest::TestRunner` and calling `atest::TestRunner::run()`. The reason for this is two-fold:

-   `TestRunner` requires main's arguments (`int argc, char *argv[]`) that controls filtering, reporting etc.
-   `TestRunner` optionally accepts a stream object to print to (see [Printer](#printer)).
-   The tests must be executed within the context of main (or before it) otherwise the code coverage (e.g. `llvm-cov`) and other instrumentation based tools (e.g. sanitizers) will not work properly. These are typically used with the tests so this property is important. **Therefore it is not feasible to run tests in a destructor of a global/static variable for example.**

There is no parallelization within a test run. At any one time there is only one test suite and one test from that test suite being executed. The global nameless test suite is always executed first. The user defined test suites are executed in order defined by their names (e.g. `a suite`, `b suite`, `my suite` etc.). All tests within a test suite are executed sequentially in order of declaration.

---

_NOTE_
The `atest::TestRunner::run()` is **not thread-safe** but it **is re-entrant**. However the executed tests or the code under test might not be re-entrant.

---

The `atest::TestRuner::run()` will return number of failures which should be `0` if all tests passed or a number greater than `0` if there was a failure. No exceptions are propagated from running the tests themselves but it is still possible for `run()` to throw. If that happens something seriously wrong happened and the ultimate call to `std::terminate()/std::abort()` performed by default is probably the right thing to happen. If you want the run never to fail and handle such situation yourself wrap the call to the `atest::TestRunner::run()` in a `try {} catch (...) {}` block.

Example:

```
import atest;
auto main(int argc, char *argv[]) -> int
{
    return ::atest::TestRunner{argc, argv}.run();
}
```

Custom stream example:

```
import atest;
auto main(int argc, char *argv[]) -> int
{
    std::stringstream testOutputStream;
    return ::atest::TestRunner{argc, argv, &testOutputStream}.run();
}
```

## Printer

By default, the `atest` outputs the progress and results to `std::cout`. It is possible to change the output stream by passing the `std::ostream *` as an additional argument to the [Test Runner](#test-runner)).

It is also a requirement for all values used in the expectations and matching to be printable. A printable value is any value for which `auto operator<<(std::ostream &stream, const T &value) -> std::ostream &` exists. If the operator does not exist it will result in a compiler error when compiling the test. The containers that have `begin()` and `end()` are automatically printed as arrays and thus only the internal type `T` might need to be made printable. A custom printing of the whole containers can still be provided by the user.

## Filtering & Listing

The tests can be listed, selected or filtered using the optional command line arguments:

```
-?                             Display the help.
--list, -l                     Lists tests. Can be combined with filters.
--test=pattern, -t=pattern     Runs only the tests that matches the pattern.
--suite-pattern, -s=pattern    Runs only test suites that matches the pattern.
--filter-test=pattern          Skips tests that matches the pattern value.
--filter-suite=pattern         Skips test suites that matches the pattern value.
```

The pattern supports only leading and trailing wildcard `*`, e.g.

-   `my test` will match only test (or suite) that match exactly the pattern `my test`
-   `test*` will match any test (or suite) that starts with the word `test`
-   `*test` will match any test (or suite) that ends with the word `test`

The options can be combined. So for example if the two test suites both contain test `my test` you can select the desired test by combining `-t "my test" -s "suite1"`.

# Known Issues

-   Using `clang-tidy` with global calls to `suite()` will result in incorrect `cert-err58-cpp` warning being emitted stating that the function may throw and there is no possibility to catch the exception. The function is however declared `noexcept` and its body is wrapped in `try {} catch (...) {}` therefore it cannot actually throw any exceptions. It indicates success by returning `0` and failure (an exception) byt returing `1`. To avoid this you need either to add `//NOLINT(cert-err58-cpp)` to the line where you call `suite()` or call `suite()` directly or indirectly from `main()` only.
