export module atest;

export import astl;

#ifndef __clang__
export import : test_runner;
export import : expect;
#else
// clang-format off
#include "stringify.cpp" //NOLINT(bugprone-suspicious-include)
#include "failed_assertion.cpp" //NOLINT(bugprone-suspicious-include)
#include "failure.cpp" //NOLINT(bugprone-suspicious-include)
#include "test.cpp" //NOLINT(bugprone-suspicious-include)
#include "test_suite.cpp" //NOLINT(bugprone-suspicious-include)
#include "test_context.cpp" //NOLINT(bugprone-suspicious-include)
#include "matcher_base.cpp" //NOLINT(bugprone-suspicious-include)
#include "matcher.cpp" //NOLINT(bugprone-suspicious-include)
#include "matcher_contains.cpp" //NOLINT(bugprone-suspicious-include)
#include "expect_base.cpp" //NOLINT(bugprone-suspicious-include)
#include "expect_to_match.cpp" //NOLINT(bugprone-suspicious-include)
#include "expect_to_throw.cpp" //NOLINT(bugprone-suspicious-include)
#include "expect.cpp" //NOLINT(bugprone-suspicious-include)
#include "stats.cpp" //NOLINT(bugprone-suspicious-include)
#include "results.cpp" //NOLINT(bugprone-suspicious-include)
#include "reporter.cpp" //NOLINT(bugprone-suspicious-include)
#include "printer.cpp" //NOLINT(bugprone-suspicious-include)
#include "test_runner.cpp" //NOLINT(bugprone-suspicious-include)
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
[[nodiscard]] auto assert_(const ExpressionT &expression, // NOLINT(readability-identifier-naming)
                           const std::source_location &sourceLocation = std::source_location::current()) noexcept
    -> Expect<ExpressionT,
              ExpectationType::Assertion,
              ResultHandlingPolicy::Normal>
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
              ResultHandlingPolicy::Reverse>
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
              ResultHandlingPolicy::Normal>
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
              ResultHandlingPolicy::Reverse>
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
    ::atest::test_context().current_test_suite().tests.emplace_back(Test{name, body, sourceLocation});
}

//! Registers the test suite under `name`. The
//! suite's `body` will be run immediately. Any
//! test() call inside will register the test to
//! the suite. The suite() calls cannot be nested.
//! This function will never throw. It returns 0
//! on success, 1 otherwise. The call site source
//! location is automatically captured.
export template<typename T = int>
auto suite(const char *name,
           auto (*body)()->void,
           const std::source_location &sourceLocation = std::source_location::current()) noexcept -> int
{
    return ::atest::test_context().add_test_suite(name, body, sourceLocation);
}
}
