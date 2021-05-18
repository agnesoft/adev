#ifdef _MSC_VER
module atest : reporter;
import : data;
#endif

namespace atest
{
//! The `Reporter` aggregates test results.
//!
//! It will generate a Report with various statistics
//! such as number of tests, expectations, failures and
//! the tests' duration from the list of test suites.
class Reporter
{
public:
    //! Generates the Report from the `testSuites` by aggregating statistics
    //! from each test.
    [[nodiscard]] static auto generateReport(const std::vector<TestSuite> &testSuites) -> Report
    {
        Report report;
        report.testSuites = testSuitesCount(testSuites);

        for (const TestSuite &testSuite : testSuites)
        {
            reportTestSuite(&report, testSuite);
        }

        return report;
    }

    //! Generates the Report from the `testSuites` that only contain basic
    //! information such as number of test suites and number of tests.
    [[nodiscard]] static auto generateStats(const std::vector<TestSuite> &testSuites) -> Report
    {
        Report report;
        report.testSuites = testSuitesCount(testSuites);
        report.tests = std::accumulate(testSuites.begin(), testSuites.end(), size_t{0}, [](size_t count, const TestSuite &testSuite) { return count + testSuite.tests.size(); });
        return report;
    }

private:
    static auto reportTest(Report *report, const Test &test) -> void
    {
        report->expectations += test.expectations;
        report->duration += test.duration;

        if (!test.failures.empty())
        {
            report->failedTests++;
            report->failedExpectations += test.failedExpectations;
            report->failures += test.failures.size();
        }
    }

    static auto reportTestSuite(Report *report, const TestSuite &testSuite) -> void
    {
        report->tests += testSuite.tests.size();

        for (const Test &test : testSuite.tests)
        {
            reportTest(report, test);
        }
    }

    [[nodiscard]] static auto testSuitesCount(const std::vector<TestSuite> &testSuites) -> size_t
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
