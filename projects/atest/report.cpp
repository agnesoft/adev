#ifndef __clang__
module atest : report;
import astl;
#endif

namespace atest
{
//! Aggregate results of the test run.
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
}
