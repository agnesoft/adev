#ifdef _MSC_VER
module atest : printer;

import : data;
import : stringify;
#endif

namespace atest
{
class Printer
{
public:
    explicit Printer(std::ostream *stream) noexcept :
        mStream{stream}
    {
    }

    auto beginRun(const Report &report) -> void
    {
        stream() << "Running " << report.tests << " tests from " << report.testSuites << " test suites...\n"
                 << separator() << "\n\n";
    }

    auto beginTest(const Test *test) -> void
    {
        constexpr int EXTRA_WIDTH = 6;
        stream() << indent() << std::left << std::setw(mTestWidth + EXTRA_WIDTH) << stringify(sourceLocationToString(test->sourceLocation), " \"", test->name, "\"...");
        mIndentLevel++;
    }

    auto beginTestSuite(const TestSuite *testSuite) -> void
    {
        stream() << sourceLocationToString(testSuite->sourceLocation) << " \"" << testSuite->name << "\":\n";
        mTestWidth = testsWidth(testSuite);
        mIndentLevel++;
    }

    auto endRun(const Report &report) -> void
    {
        const int width = static_cast<int>(std::to_string(std::max(report.tests, report.expectations)).size());
        const int passedWidth = static_cast<int>(std::to_string(std::max(report.failedTests, report.failedExpectations)).size());

        stream() << separator() << "\n"
                 << "Result      : " << (report.failedTests == 0 ? std::string_view{"PASSED"} : std::string_view{"FAILED"}) << '\n'
                 << "Duration    : " << std::chrono::duration_cast<std::chrono::milliseconds>(report.duration).count() << "ms\n"
                 << "Tests       : " << std::left << std::setw(width) << report.tests << " | "
                 << std::left << std::setw(passedWidth) << (report.tests - report.failedTests) << " passed | "
                 << report.failedTests << " failed\n"
                 << "Expectations: " << std::left << std::setw(width) << report.expectations << " | "
                 << std::left << std::setw(passedWidth) << (report.expectations - report.failedExpectations) << " passed | "
                 << report.failedExpectations << " failed\n";
    }

    auto endTest(const Test *test) -> void
    {
        if (test->failures.empty())
        {
            stream() << " [PASSED] " << std::chrono::duration_cast<std::chrono::milliseconds>(test->duration).count() << "ms\n";
        }
        else
        {
            stream() << " [FAILED] " << std::chrono::duration_cast<std::chrono::milliseconds>(test->duration).count() << "ms\n";
            printTestFailures(test);
        }

        mIndentLevel--;
    }

    auto endTestSuite([[maybe_unused]] const TestSuite *testSuite) -> void
    {
        stream() << '\n';
        mIndentLevel--;
    }

    auto print(const std::string &prefix, const std::string &text) -> void
    {
        if (!text.empty())
        {
            stream() << indent() << prefix << ' ' << text << '\n';
        }
    }

private:
    [[nodiscard]] static auto sourceLocationToString(const source_location<> &location) -> std::string
    {
        return stringify(std::filesystem::path{location.file_name()}.filename().string(), ':', location.line(), ':');
    }

    [[nodiscard]] auto indent() const -> std::string
    {
        return std::string(mIndentLevel * 2, ' ');
    }

    auto printTestFailure(const Failure &failure) -> void
    {
        print(stringify("at ", sourceLocationToString(failure.sourceLocation)), failure.what);
        print("  Expected: ", failure.expected);
        print("  Actual  : ", failure.actual);
    }

    auto printTestFailures(const Test *test) -> void
    {
        for (const Failure &failure : test->failures)
        {
            printTestFailure(failure);
        }
    }

    [[nodiscard]] static auto separator() -> std::string
    {
        constexpr size_t SEPARATOR_WIDTH = 75;
        return std::string(SEPARATOR_WIDTH, '=');
    }

    [[nodiscard]] auto stream() noexcept -> std::ostream &
    {
        return *mStream;
    }

    [[nodiscard]] static auto testNamesWidth(const TestSuite *testSuite) -> int
    {
        size_t width = 0;

        for (const Test &test : testSuite->tests)
        {
            if (width < test.name.size())
            {
                width = test.name.size();
            }
        }

        return static_cast<int>(width);
    }

    [[nodiscard]] static auto testsWidth(const TestSuite *testSuite) -> int
    {
        return static_cast<int>(sourceLocationToString(testSuite->tests.back().sourceLocation).size()) + testNamesWidth(testSuite);
    }

    std::ostream *mStream = nullptr;
    int mIndentLevel = 0;
    int mTestWidth = 0;
};
}
