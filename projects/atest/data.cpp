#ifdef _MSC_VER
module atest : data;

import : source_location;
import "astl.hpp";
#endif

namespace atest
{
struct Failure
{
    std::string what;
    std::string expected;
    std::string actual;
    source_location<> sourceLocation;
};

struct Test
{
    std::string name;
    auto (*testBody)() -> void = nullptr;
    source_location<> sourceLocation;
    size_t expectations = 0;
    size_t failedExpectations = 0;
    std::chrono::microseconds duration = std::chrono::microseconds::zero();
    std::vector<Failure> failures;
};

struct TestSuite
{
    std::string name;
    source_location<> sourceLocation;
    std::vector<Test> tests;
};

struct Tests
{
    std::vector<TestSuite> suites = {TestSuite{"Global"}};
    TestSuite *currentTestSuite = &suites.front();
    Test *currentTest = nullptr;
};

struct Report
{
    size_t testSuites = 0;
    size_t tests = 0;
    size_t failedTests = 0;
    size_t expectations = 0;
    size_t failedExpectations = 0;
    std::chrono::microseconds duration = std::chrono::microseconds::zero();
};

//! \private
[[nodiscard]] auto globalTests() -> Tests *
{
    static Tests tests;
    return &tests;
}
}
