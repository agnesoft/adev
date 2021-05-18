#ifdef _MSC_VER
module atest : data;
import<astl.hpp>;
#endif

namespace atest
{
//! Exception type to indicate an assertion failed.
struct FailedAssertion
{
};

//! Failure and its details.
struct Failure
{
    std::string what;
    std::string expected;
    std::string actual;
    std::source_location sourceLocation;
};

//! Test and its results.
struct Test
{
    std::string name;
    auto (*testBody)() -> void = nullptr;
    std::source_location sourceLocation;
    size_t expectations = 0;
    size_t failedExpectations = 0;
    std::chrono::microseconds duration = std::chrono::microseconds::zero();
    std::vector<Failure> failures;
};

//! Test suite with its registered tests.
struct TestSuite
{
    std::string name;
    std::source_location sourceLocation;
    std::vector<Test> tests;
};

//! Registered test suites as well as currently run/registered test suite and test.
struct Tests
{
    std::vector<TestSuite> suites = {TestSuite{"Global"}};
    TestSuite *currentTestSuite = &suites.front();
    Test *currentTest = nullptr;
};

//! Aggregate report of a test run.
struct Report
{
    size_t testSuites = 0;
    size_t tests = 0;
    size_t failedTests = 0;
    size_t expectations = 0;
    size_t failedExpectations = 0;
    size_t failures = 0;
    std::chrono::microseconds duration = std::chrono::microseconds::zero();
};

//! Returns the global Tests object for registering test suites and tests and to run them.
[[nodiscard]] auto globalTests() -> Tests *
{
    static Tests tests;
    return &tests;
}
}
