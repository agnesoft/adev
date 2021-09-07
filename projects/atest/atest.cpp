export module atest;

export import astl;

#ifndef __clang__
export import : test_runner;
export import : expect;
#else
// clang-format off
#include "stringify.cpp"
#include "failed_assertion.cpp"
#include "failure.cpp"
#include "test.cpp"
#include "test_suite.cpp"
#include "global_tests.cpp"
#include "matcher_base.cpp"
#include "matcher.cpp"
#include "expect_base.cpp"
#include "expect_to_match.cpp"
#include "expect_to_throw.cpp"
#include "expect.cpp"
#include "report.cpp"
#include "reporter.cpp"
#include "printer.cpp"
#include "test_runner.cpp"
// clang-format on
#endif

//! atest namespace
namespace atest
{
//! Starts an assertion that will stop the test
//! when failed. The `expression` can be a
//! callable. The call site source location is
//! automatically captured.
export template<typename ExpressionT>
[[nodiscard]] auto assert_(const ExpressionT &expression,
                           const std::source_location &sourceLocation = std::source_location::current()) noexcept
    -> Expect<ExpressionT,
              ExpectationType::Assertion,
              FailurePolicy::FailOnFailure>
{
    return {expression, sourceLocation};
}

//! Starts an assertion with reversed result that
//! will stop the test when failed. The
//! `expression` can be a callable. The call site
//! source location is automatically captured.
export template<typename ExpressionT>
[[nodiscard]] auto assert_fail(const ExpressionT &expression,
                               const std::source_location &sourceLocation = std::source_location::current()) noexcept
    -> Expect<ExpressionT,
              ExpectationType::Assertion,
              FailurePolicy::PassOnFailure>
{
    return {expression, sourceLocation};
}

//! Starts an expectation. The `expression` can be
//! a callable. The call site source location is
//! automatically captured.
export template<typename ExpressionT>
[[nodiscard]] auto expect(const ExpressionT &expression,
                          const std::source_location &sourceLocation = std::source_location::current()) noexcept
    -> Expect<ExpressionT,
              ExpectationType::Expectation,
              FailurePolicy::FailOnFailure>
{
    return {expression, sourceLocation};
}

//! Starts an expectation with reversed result.
//! The `expression` can be a callable. Call site
//! source location is automatically captured.
export template<typename ExpressionT>
[[nodiscard]] auto expect_fail(const ExpressionT &expression,
                               const std::source_location &sourceLocation = std::source_location::current()) noexcept
    -> Expect<ExpressionT,
              ExpectationType::Expectation,
              FailurePolicy::PassOnFailure>
{
    return {expression, sourceLocation};
}

//! Registers the test's `body` under `name` in
//! the current test suite() (or the global test
//! suite if the test is not declared inside
//! suite() call). The `body` will be executed
//! during the execution of
//! atest::TestRunner::run(). The call site source
//! location is automatically captured.
export template<typename T = int>
auto test(const char *name,
          auto (*body)()->void,
          const std::source_location &sourceLocation = std::source_location::current()) -> void
{
    ::atest::global_tests().currentTestSuite->tests.emplace_back(Test{name, body, sourceLocation});
}

//! Registers the test suite under `name`. The suite's
//! `body` will be run immediately. Any
//! test() called inside will register the test to
//! the suite. The suite() calls cannot be nested.
//! This function will never throw. It returns 0
//! on success, 1 otherwise. The call site source
//! location is automatically captured.
export template<typename T = int>
auto suite(const char *name,
           auto (*body)()->void,
           const std::source_location &sourceLocation = std::source_location::current()) noexcept -> int
{
    try
    {
        try
        {
            GlobalTests &tests = ::atest::global_tests();
            tests.currentTestSuite = &tests.suites.emplace_back(TestSuite{name, sourceLocation});
            body();
            tests.currentTestSuite = &tests.suites.front();
            return 0;
        }
        catch (const std::exception &exception)
        {
            std::cout << "ERROR: Exception thrown during registration of '"
                      << name
                      << "' test suite ("
                      << typeid(exception).name()
                      << "): " << exception.what()
                      << '\n';
        }
        catch (...)
        {
            std::cout << "ERROR: Unknown exception thrown during registration of '"
                      << name
                      << "' test suite.\n";
        }
    }
    catch (...)
    {
        //Suppress any further exceptions as this
        //function is usually run outside of main
        //and no exception can be caught. See
        //clang-tidy check: cert-err58-cpp.
    }

    return 1;
}
}
