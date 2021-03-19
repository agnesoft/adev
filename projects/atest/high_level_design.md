# High Level Design

-   [Problem](#problem)
-   [Requirements](#requirements)
-   [Existing Solutions](#existing-solutions)
-   [ATest](#atest)
    -   [test()](#test)
    -   [expect()](#expect)
    -   [e](#e)

## Problem

"If you liked it, then you should have put a test on it." -Beyoncé rule

Test is a software written to validate assumptions about another software and is essential for both its development and maintenance. Testing frameworks aim to facilitate writing tests by providing common functionality such as bootstrapping, validation or reporting.

## Requirements

A modern C++ testing framework should:

-   Be simple to write tests in.
-   Be extensible.
-   Be a C++20 module.
-   Not use macros.

## Existing Solutions

There are many C++ testing frameworks written over the years. The most popular is [Google Test](https://github.com/google/googletest) (a.k.a. GTest) that is however heavily macro based. Another more recent addition is [Catch2](https://github.com/catchorg/Catch2) that is also macro based and is plagued by the fact that it is single-header with significant compile time overhead. One of the most recent additions is a prorotype [μt](https://github.com/boost-ext/ut) that is specifically designed with the above requirements in mind - unfortunately it does not work as a module. All the testing frameworks are rather heavy handed in an attempt to support as many styles and functionality as possible. As a result they often become bloated and hard to use, learn and very hard to extend.

## ATest

The `ATest` is supposed to be lightweight minimalist C++20 module testing library without macros.

Example:

```
//...
import atest;

int main() {
    using namespace atest;

    test("Test against literal", [] {
        expect(2_e == sum(1, 1));
    });

    test("Test against dynamic object, [] {
        const std::vector<int> expected = {1, 2, 3, 4};
        const actual = produce();

        expect(e{expected} == actual);
    })

    test("Test single value", [] {
        expect(e{std::is_standard_layout_v<SomeType>});
    })

    test("Exception", [] {
        expect(throws([] { produce() }, "Exception message"));
    })
}
```

### test()

### expect()

### e
