#ifndef __clang__
module atest : reporter;
import : results;
import : test_suite;
#endif

namespace atest
{
//! \private
class Reporter
{
public:
    [[nodiscard]] static auto results(const std::vector<TestSuite> &testSuites) -> Results
    {
        Results results{
            Stats{.testSuites = Reporter::test_suites_count(testSuites)}};

        for (const TestSuite &testSuite : testSuites)
        {
            Reporter::report_test_suite(testSuite, results);
        }

        return results;
    }

    [[nodiscard]] static auto stats(const std::vector<TestSuite> &testSuites) -> Stats
    {
        return Stats{.testSuites = Reporter::test_suites_count(testSuites),
                     .tests = std::accumulate(testSuites.begin(),
                                              testSuites.end(),
                                              std::size_t{0},
                                              [](std::size_t count, const TestSuite &testSuite) {
                                                  return count + testSuite.tests.size();
                                              })};
    }

private:
    static auto report_test(const Test &test, Results &results) -> void
    {
        results.expectations += test.expectations;
        results.duration += test.duration;

        if (!test.failures.empty())
        {
            results.failedTests++;
            results.failedExpectations += test.failedExpectations;
            results.failures += test.failures.size();
        }
    }

    static auto report_test_suite(const TestSuite &testSuite, Results &results) -> void
    {
        results.tests += testSuite.tests.size();

        for (const Test &test : testSuite.tests)
        {
            Reporter::report_test(test, results);
        }
    }

    [[nodiscard]] static auto test_suites_count(const std::vector<TestSuite> &testSuites) -> std::size_t
    {
        if (testSuites[0].tests.empty())
        {
            return testSuites.size() - 1;
        }

        return testSuites.size();
    }
};
}
