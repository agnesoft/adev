# ATest

C++ testing framework.

-   [Dependencies](#dependencies)
-   [Reference](#reference)
    -   [test()](#test)
    -   [suite()](#suite)
    -   [expect()](#expect)
        -   [toBe()](#tobe)
        -   [toMatch()](#tomatch)
        -   [toThrow()](#tothrow)
        -   [toFail()](#tofail)
    -   [Test Runner](#test-runner)
    -   [Printer](#printer)
-   [Build](#build)
    -   [MSVC](#msvc)
    -   [clang](#clang)
-   [Known Issues](#known-issues)
    -   [Affects Users](#affects-users)
    -   [Does Not Affect Users](#does-not-affect-users)

## Dependencies

-   C++20
-   [ASTL](../astl/README.md)

## Reference

### test()

```
auto atest::test(const char *name, auto (*testBody)()->void) -> int
```

Registers a test `testBody` under the name `name`. It can be called either within a [test suite](#suite) or in the `main()`. When not called within a `suite()` call the test will be registered in the implicit `Global` test suite. The second argument is a nullary function that takes no arguments and return nothing. It would typically be a lambda.

Example:

```
import atest;
int main()
{
    test("My test", [] {
        //...
    });
}
```

### suite()

```
auto atest::suite(const char *name, auto (*suiteBody)()->void) noexcept -> int
```

Groups tests into a test suite called `name`. It can be called in `main()` or outside of main capturing the result in a global/static variable in order to let the suite and tests be registered. The function will return `0` if it succeeds or `1` if an exception has been thrown during registration. No exceptions are propagated as the function is typically called before `main()` and thus it is not possible to capture the exceptions within such context.

Example:

```
import atest;
static auto s = suite("My test suite", [] {
    test("My test", [] {
        //...
    });
});
```

### expect()

```
template<typename T>
auto atest::expect(const T &value) noexcept -> Expect<T>
```

Starts composition of the test's expectation (assertion). Any `T` is allowed as argument. If `T` is a callable it will be executed once the assertion is finalized. No exceptions will be propagated from the call. The return value is the internal class `Expect` that lets you select from one of the supported expecations:

#### toBe()

```
template<typename V>
auto toBe(const V &value) -> ExpectToMatch
```

Completes the expectation using the default matcher that uses `operator==` to match the values. Custom types can be supported by simply defining the `operator==` for them. Note that if your type will be nested in a container such as `std::vector` its `operator==` might need to be implemented as a member function rather than a standalone function for ADL (Argument Dependent Lookup) to find it.

Examples:

```
expect(1 + 1).toBe(2); //matching values
expect([] { return 1 + 1; }).toBe(2); //matching result of a callable and a value
```

#### toMatch()

```
template<typename M, typename V>
auto toMatch(const V &value) -> ExpectToMatch
```

Completes the expectation using a custom matcher. The custom matcher can be any callable type that takes parameters of type `T` and `V` to perfrom the matching (i.e. `auto operator()(const T&, const V&) -> bool`). Furthermore the matcher needs to have the following methods:

-   `auto describe() -> std::string`
-   `auto expected(const T &, const V &) -> std::string`
-   `auto actual(const T &, const V &) -> std::string`

These methods are used for reporting a failure in case it occurs. The first function gives description of the expectation or of the operation the custom matcher was performing. The other two lets you customize the output for the `expected` and `actual` outcomes of the match and will be given the actual values. You can inherit from `atest::MatcherBase` if any or all of these functions do not need to be specialised for your matcher. If you are implementing them yourself you may use the convenience `auto atest::stringify(const T &...values) -> std::string` to convert `T`, `V` or anything else to strings (requires `auto operator<<(std::ostream &, const X &value) -> std::ostream &` to be implemented for the type). The `stringify` knows how to print containers so you only need the operator for the inner type.

Example:

Minimal matcher:

```
struct MyMatcher : atest::MatcherBase
{
    auto operator()(int left, int right) -> bool { return left < right; }
};
```

Full matcher:

```
struct MyMatcher
{
    auto describe() -> std::string { return "Left value to be less than right value."; }
    auto expected(int left, int right) -> std::string { return std::to_string(left) + " < " + std::to_string(right) }
    auto actual(int left, int right) -> std::string { return std::to_string(left) + (left == right ? std::string{" = "} : std::string{" > "}) + std::to_string(right); }
    auto operator()(int left, int right) -> bool { return left < right; }
};

expect(1).toMatch<MyMatcher>(2);
```

#### toThrow()

```
template<typename E>
auto toThrow() -> ExpectToThrow

template<typename E>
auto toThrow(const std::string &exceptionText) -> ExpectToThrow

template<typename E>
auto toThrow(const E &exception) -> ExpectToThrow
```

Completes the matching by expecting an exception. The `T` passed to the `expect()` must be a callable for this expectation. The exception must be specified as a template argument to the call. Optionally an expected exception text can be passed. The type matching of the exception thrown and expected exception is performed using `typeid` (RTTI) and the exact match is required (i.e. expecting a base class but throwing a derived class is a failure).

For the text matching to work the exception `E` must provide a method `what()` that returns a string. This would typically be satisfied when deriving from `std::exception` but it is not required.

The third overload lets you match the exception object directly which would require `operator==` for the type. Strict type equality of the types is still performed. Additionally the exception type must be printable (requires `auto operator<<(std::ostream &, const E &exception) -> std::ostream &` to be implemented).

Examples:

```
expect([] { throw std::exception{}; }).toThrow<std::exception>();
expect([] { throw std::logic_error{"Some text"}; }).toThrow<std::logic_error>("Some text");
expect([] { throw 1; }).toThrow<int>();
expect([] { throw 1; }).toThrow(1);
```

#### toFail()

```
auto toFail() -> void
auto toFail(const T &actual) -> void
```

Optionally modifies the expectation to reverse the result. If the expectation passes it will be converted into an error. Conversely if the expectation fails it will be considered a success. No additional output will be printed regarding the failure. It is primarily useful for testing negative scenarios.

The second overload can additionally validate the failure against an arbitrary value by stringifying it and comparing it to the `actual` field of the failure (see [toMatch()](#tomatch) for details regarding matchers).

Example:

```
expect(1).toBe(2).toFail(); //passes despite failing
expect(1).toBe(2).toFail(1); //validates the failure against value "1"
expect(1).toBe(2).toFail(3); //still fails because failure was not successfully validated
```

### Test Runner

The test runner is the main entry point of `atest`. The registrations of the test suites and tests are global and the user is responsible to actuall start the test run by instantiating and calling `atest::TestRunner::run()`. The reason for this is two-fold:

-   `TestRunner` optionally accepts main's argumnets (`int argc, char *argv[]`) that controls filtering, reporting etc. as well as the stream for the [Printer](#printer).
-   The tests must be executed within the context of main (or before it) otherwise the code coverage (e.g. `llvm-cov`) and other instrumentation based tools (e.g. sanitizers) will not work properly. These are typically used with the test.

The `run()` method of the `atest::TestRuner` will return `0` if all tests passed or `1` if there was a failure. No exceptions are propagated from running the tests themselves but it is still possible for `run()` to throw. If that happens something seriously wrong occured and the ultimate call to `std::termina()/std::abort()` performed by default is probably right thing to happen. If you want the run never to fail and handle such situation yourself wrap the call to `atest::TestRunner::run()` in a `try {} catch (...) {}` block.

Examples:

Minimal

```
import atest;
int main()
{
    return atest::TestRunner{}.run();
}
```

Full

```
import atest;
int main(int argc, char *argv[])
{
    std::stringstream testOutputStream;
    return atest::TestRunner{argc, argv, &testOutputStream}.run();
}
```

### Printer

By default, the `atest` outputs the progress and results to `std::cout`. It is possible to change the output stream by passing the `std::ostream *` as an additional argumen to the [Test Runner](#test-runner)).

It is also a requirement for all values used in the expectations and matching to be printable. A printable value is any value for which `auto operator<<(std::ostream &stream, const T &value) -> std::ostream &` exists. If the operator does not exist it will result in a compiler error when compiling the test. The containers that have `begin()` and `end()` are automatically printed as arrays and thus only the internal type `T` might need to be made printable. A custom printing of the whole containers can still be provided by the user.

## Build

### MSVC

```
cl /std:c++latest /headerUnit "project/astl/include/astl.hpp=astl.hpp.ifc" /c /Fo /interface atest.ixx
cl /std:c++latest /headerUnit "project/astl/include/astl.hpp=astl.hpp.ifc" test\atest_test.cpp atest.obj astl.obj
```

Everything must be compiled with the same flags that affect code generation including the `astl` dependency.

The `/interface` flag will generate the `ifc` (compiled module interface) for `atest`. Finally the `atest.ifc` does not need to be specified when compiling a translation unit that imports it if it can be found in the same directory. Header units mapping however still must be specified for each translation unit even transitively (like `astl.hpp=astl.hpp.ifc`). Note that both header units and the module interfaces must also produce regular object files (`/Fo`) so that the code can be linked together.

-   `/exportHeader` will compile a header into an `ifc` (a header unit).
-   `/interface` will compile a module into the `ifc`.
-   `/std:c++latest` enabled C++20 modules.
-   `/Fo` to produce an object file.
-   `/c` to perform compilation only.
-   `/TP` means that hte input files are all C++ source files.

### Clang

Using `clang++`:

```
mkdir -p build
cd build

clang++-11 -std=c++2a -I../astl/include -fmodules -Xclang -emit-module-interface -c -x c++ atest.ixx -o atest.pcm
clang++-11 -std=c++2a -I../astl/include -fmodules -fprebuilt-module-path=. -c -x c++ atest.ixx -o atest.o
clang++-11 -std=c++2a -I../astl/include -L<libc++pprefix>/lib -lc++ -fmodules -fprebuilt-module-path=. `-Wl,-rpath,<libcxx-install-prefix>/lib` test/atest_test.cpp atest.o -o atest_test

cd ..
```

Everything must be compiled with the same flags that affect code generation. The first command will compile a module interface into a precompiled module `pcm`. The second command will generate a regular object file from the module with the symbols used for linking purposes. The third command will compile the source file that imports the module and will also link to its object file.

-   `-std=c++2a` enables modules.
-   `-I../astl/include` adds `astl` dependency's include path that include its header and the `moduel.moduelmap` files.
-   `-fmodules` enables modules and implicit header unit creation and implicit module maps.
-   `-Xclang -emit-module-interface` compiles a module into `pmc`. Notice the flag needs to be forwarded to the compiler with `-Xclang`.
-   `-c` tells clang to only perform compilation and no linking.
-   `-x c++` tells clang to treat the following input file as C++ file. Required for non-standard extensions such as `*.ixx`.
-   `-fprebuilt-module-path=.` tells clang where to look for precompiled modules - int this case the current directory.
-   `-L<libc++pprefix>/lib` adds the `libc++` lib directory to the library search path for linking.
-   `-lc++` tells clang to link against `libc++`.
-   `-Wl,-rpath,<libcxx-install-prefix>/lib` embeds the `libc++` library path to the binary, so it can be found during runtime.

## Known Issues

### Affects Users

-   Using templated function such as `toThrow` (e.g. `expect([] { throw 1; }).toThrow<int>();` does not work and requires `template` keyword (e.g. `expect([] { throw 1; }).template toThrow<int>();`) otherwise parser fails: Reported bug: https://developercommunity.visualstudio.com/t/Cannot-instantiate-template-exported-fro/1387974
-   Using `clang-tidy` with global calls to `suite()` will result in incorrect `cert-err58-cpp` warning being emitted stating that the function may throw and there is no possibility to catch the exception. The function is however declared `noexcept` and its body is wrapped in `try {} catch (...) {}` therefore it cannot actually throw any exceptions. It indicates succes by returning `0` and failure (an exception) byt returing `1`. To avoid this you need either to add `//NOLINT(cert-err58-cpp)` to the line where you call `suite()` or call `suite()` directly or indirectly from `main()` only (but the second option loses the self-registering property of the suites).

### Does Not Affect Users

-   [**All**] No STL vendor currently supports C++20 `source_location`. It is being emulated with `__builtin_FILE` that however is not `consteval` and does not really evaluate at the callsite. To force it to evaluate at the callsite the `source_location` and its callers `suite`. `test` and `expect` are all template functions so they are instantiated at the call site. Once actual `source_location` is available the custom version can be scrapped and `suite` and `test` may stop being template functions.
-   [**MSVC 16.9.3**] Using `type_info` requires `using ::type_info;` and include of `<typeinfo>` in the global module fragment, otherwise the `type_info` struct is unknown at compile time. Reported bug: https://developercommunity.visualstudio.com/t/Undefined-reference-to-type_info-when-us/1384072
-   [**libc++ 11.1.0**] C++20 `operator<<(std::chrono::duration)` is missing in `libc++` and requires a workaround in form of `duration.count() << "ms"` to emulate the operator. Should be removed once `libc++` implements this feature.
-   [**libc++ 11.1.0**] C++20 ranges are missing in `libc++` and `ranges::range<T>` concept cannot be used. It was replaced by custom `iterable` that should be removed once `libc++` implements ranges.
