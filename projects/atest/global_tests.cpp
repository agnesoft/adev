#ifndef __clang__
module atest : global_tests;
import : test_suite;
#endif

namespace atest
{
//! Collection of test suites and currently run
//! test suite and test.
struct GlobalTests
{
    std::vector<TestSuite> suites = {TestSuite{"Global"}};
    TestSuite *currentTestSuite = &suites.front();
    Test *currentTest = nullptr;
};

//! Returns the global GlobalTests object for
//! registering test suites and to run them.
[[nodiscard]] auto global_tests() -> GlobalTests &
{
    static GlobalTests tests;
    return tests;
}
}
