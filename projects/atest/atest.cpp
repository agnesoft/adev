module;

#ifdef _MSC_VER
#    include <typeinfo>
#endif

export module atest;

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 5106)
#endif

export import "astl.hpp";

#ifdef _MSC_VER
#    pragma warning(pop)
#endif

#ifdef _MSC_VER
export import : expect;
export import : test_runner;
#else
// clang-format off
#include "source_location.cpp"
#include "data.cpp"
#include "stringify.cpp"
#include "printer.cpp"
#include "reporter.cpp"
#include "matcher.cpp"
#include "expect_base.cpp"
#include "expect_tomatch.cpp"
#include "expect_tothrow.cpp"
#include "expect.cpp"
#include "test_runner.cpp"
// clang-format on
#endif

//! ATest namespace
namespace atest
{
//! Starts an assertion that will stop the test when failed.
//! This function accepts any `T` but later
//! methods called may impose additional requirements on it (e.g. for `T`
//! to be a callable). Beside the `value` this function also captures the
//! call site's source_location. Returns an Expect object to complete
//! the expectation.
export template<typename T>
[[nodiscard]] auto assert_(const T &value, const source_location<> &sourceLocation = source_location<>::current()) noexcept -> Expect<T, true, false>
{
    return Expect<T, true, false>{value, sourceLocation};
}

//! Starts an assertion with reversed result that will stop the test when failed.
//! This function accepts any `T` but later
//! methods called may impose additional requirements on it (e.g. for `T`
//! to be a callable). Beside the `value` this function also captures the
//! call site's source_location. Returns an Expect object to complete
//! the expectation.
export template<typename T>
[[nodiscard]] auto assert_fail(const T &value, const source_location<> &sourceLocation = source_location<>::current()) noexcept -> Expect<T, true, true>
{
    return Expect<T, true, true>{value, sourceLocation};
}

//! Starts an expectation. This function accepts any `T` but later
//! methods called may impose additional requirements on it (e.g. for `T`
//! to be a callable). Beside the `value` this function also captures the
//! call site's source_location. Returns an Expect object to complete
//! the expectation.
export template<typename T>
[[nodiscard]] auto expect(const T &value, const source_location<> &sourceLocation = source_location<>::current()) noexcept -> Expect<T, false, false>
{
    return Expect<T, false, false>{value, sourceLocation};
}

//! Starts an expectation with reversed result. This function accepts any `T` but later
//! methods called may impose additional requirements on it (e.g. for `T`
//! to be a callable). Beside the `value` this function also captures the
//! call site's source_location. Returns an Expect object to complete
//! the expectation.
export template<typename T>
[[nodiscard]] auto expect_fail(const T &value, const source_location<> &sourceLocation = source_location<>::current()) noexcept -> Expect<T, false, true>
{
    return Expect<T, false, true>{value, sourceLocation};
}

//! Registers the test `testBody` under `name` in the current test suite().
//! If the test is not within a test suite it will be registered to the
//! implicit global test suite. The `testBody` is not executed immediately
//! but rather during the exceution of atest::TestRunner::run().
export template<typename T = int>
auto test(const char *name, auto (*testBody)()->void, const source_location<> &sourceLocation = source_location<>::current()) -> void
{
    globalTests()->currentTestSuite->tests.emplace_back(Test{name, testBody, sourceLocation});
}

//! Registers the test suite under `name`. The `suiteBody` will be run
//! immediately presumably registering the tests inside it. Note that
//! it is not possible to execute test code within the `suiteBody` itself.
//! This function will never throw and will attempt to output the text of
//! any exception it catches if it derives from `std::exception`. It returns
//! 0 on success, 1 otherwise.
export template<typename T = int>
auto suite(const char *name, auto (*suiteBody)()->void, const source_location<> &sourceLocation = source_location<>::current()) noexcept -> int
{
#ifdef _MSC_VER
    using ::type_info;
#endif

    try
    {
        try
        {
            Tests *tests = globalTests();
            tests->currentTestSuite = &tests->suites.emplace_back(TestSuite{name, sourceLocation});
            suiteBody();
            tests->currentTestSuite = &tests->suites.front();
            return 0;
        }
        catch (std::exception &e)
        {
            std::cout << "ERROR: Exception thrown during registration of '" << name << "' test suite (" << typeid(e).name() << "): " << e.what() << '\n';
        }
        catch (...)
        {
            std::cout << "ERROR: Unknown exception thrown during registration of '" << name << "' test suite.\n";
        }
    }
    catch (...)
    {
        //Supress any further exceptions as this function is
        //usually run outside of main and no exception can
        //be caught. See clang-tidy check: cert-err58-cpp.
    }

    return 1;
}
}
