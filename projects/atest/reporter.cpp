#ifndef __clang__
module atest : reporter;
import : results;
import : test_suite;
import : test_filter;
#endif

namespace atest
{
//! \private
class Reporter
{
public:
    [[nodiscard]] static auto results(const std::vector<TestSuite> &suites, const TestFilter &filter) -> Results
    {
        Results results{Stats{.testSuites = Reporter::count_test_suites(suites, filter)}};

        for (const TestSuite &suite : suites)
        {
            if (filter.is_suite_selected(suite.name))
            {
                Reporter::report_test_suite(suite, results, filter);
            }
        }

        return results;
    }

    [[nodiscard]] static auto stats(const std::vector<TestSuite> &suites, const TestFilter &filter) -> Stats
    {
        return Stats{.testSuites = Reporter::count_test_suites(suites, filter),
                     .tests = Reporter::count_tests(suites, filter)};
    }

private:
    [[nodiscard]] static auto count_test(const TestSuite &suite, const TestFilter &filter) -> std::size_t
    {
        std::size_t count = 0;

        for (const Test &test : suite.tests)
        {
            if (filter.is_test_selected(test.name))
            {
                ++count;
            }
        }

        return count;
    }

    [[nodiscard]] static auto count_test_suites(const std::vector<TestSuite> &suites, const TestFilter &filter) -> std::size_t
    {
        std::size_t count = 0;

        for (const TestSuite &suite : suites)
        {
            if (filter.is_suite_selected(suite.name) && !suite.tests.empty())
            {
                ++count;
            }
        }

        return count;
    }

    [[nodiscard]] static auto count_tests(const std::vector<TestSuite> &suites, const TestFilter &filter) -> std::size_t
    {
        std::size_t count = 0;

        for (const TestSuite &suite : suites)
        {
            if (filter.is_suite_selected(suite.name))
            {
                count += Reporter::count_test(suite, filter);
            }
        }

        return count;
    }

    static auto report_test(const Test &test, Results &results) -> void
    {
        ++results.tests;
        results.expectations += test.expectations;
        results.duration += test.duration;

        if (!test.failures.empty())
        {
            results.failedTests++;
            results.failedExpectations += test.failedExpectations;
            results.failures += test.failures.size();
        }
    }

    static auto report_test_suite(const TestSuite &suite, Results &results, const TestFilter &filter) -> void
    {
        for (const Test &test : suite.tests)
        {
            if (filter.is_test_selected(test.name))
            {
                Reporter::report_test(test, results);
            }
        }
    }
};
}
