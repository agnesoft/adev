#ifndef __clang__
module atest : reporter;
import : results;
import : selected_tests;
#endif

namespace atest
{
//! \private
class Reporter
{
public:
    [[nodiscard]] static auto results(const std::vector<SelectedTests> &tests) -> Results
    {
        Results results{Stats{.testSuites = tests.size()}};

        for (const SelectedTests &suite : tests)
        {
            Reporter::report_test_suite(suite, results);
        }

        return results;
    }

    [[nodiscard]] static auto stats(const std::vector<SelectedTests> &tests) -> Stats
    {
        return Stats{.testSuites = tests.size(),
                     .tests = Reporter::count_tests(tests)};
    }

private:
    [[nodiscard]] static auto count_tests(const std::vector<SelectedTests> &tests) -> std::size_t
    {
        std::size_t count = 0;

        for (const SelectedTests &suite : tests)
        {
            count += suite.tests.size();
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

    static auto report_test_suite(const SelectedTests &suite, Results &results) -> void
    {
        for (const Test *test : suite.tests)
        {
            Reporter::report_test(*test, results);
        }
    }
};
}
