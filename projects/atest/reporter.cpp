#ifndef __clang__
module atest : reporter;
import : report;
import : test_suite;
#endif

namespace atest
{
//! \private
class Reporter
{
public:
    [[nodiscard]] static auto generate_report(const std::vector<TestSuite> &testSuites) -> Report
    {
        Report report{
            .testSuites = Reporter::test_suites_count(testSuites)};

        for (const TestSuite &testSuite : testSuites)
        {
            Reporter::report_test_suite(report, testSuite);
        }

        return report;
    }

    [[nodiscard]] static auto generate_stats(const std::vector<TestSuite> &testSuites) -> Report
    {
        Report report{
            report.testSuites = Reporter::test_suites_count(testSuites),
            report.tests = std::accumulate(testSuites.begin(),
                                           testSuites.end(),
                                           size_t{0},
                                           [](size_t count, const TestSuite &testSuite) {
                                               return count + testSuite.tests.size();
                                           })};
        return report;
    }

private:
    static auto report_test(Report &report, const Test &test) -> void
    {
        report.expectations += test.expectations;
        report.duration += test.duration;

        if (!test.failures.empty())
        {
            report.failedTests++;
            report.failedExpectations += test.failedExpectations;
            report.failures += test.failures.size();
        }
    }

    static auto report_test_suite(Report &report, const TestSuite &testSuite) -> void
    {
        report.tests += testSuite.tests.size();

        for (const Test &test : testSuite.tests)
        {
            Reporter::report_test(report, test);
        }
    }

    [[nodiscard]] static auto test_suites_count(const std::vector<TestSuite> &testSuites) -> size_t
    {
        if (testSuites[0].tests.empty())
        {
            return testSuites.size() - 1;
        }
        else
        {
            return testSuites.size();
        }
    }
};
}
