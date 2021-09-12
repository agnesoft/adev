#ifndef __clang__
module atest : global_tests;
import : test_suite;
#endif

namespace atest
{
//! \private
struct GlobalTests
{
    std::vector<TestSuite> suites = {TestSuite{"Global"}};
    TestSuite *currentTestSuite = &suites.front();
    Test *currentTest = nullptr;
};

//! \private
[[nodiscard]] auto global_tests() -> GlobalTests &
{
    static GlobalTests tests;
    return tests;
}
}
