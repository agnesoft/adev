#ifdef _MSC_VER
module atest : printer;
import : data;
import : stringify;
#endif

namespace atest
{
//! Color codes for ANSI console foreground colors.
struct Color
{
    static constexpr char GRAY[] = "\033[1;30m";
    static constexpr char GREEN[] = "\033[1;32m";
    static constexpr char RED[] = "\033[1;31m";
    static constexpr char RESET[] = "\033[0m";
    static constexpr char YELLOW[] = "\033[1;33m";
};

//! The `Printer` class formats output to a `std::ostream`.
//!
//! It converts raw test data into human readable formatted output.
class Printer
{
public:
    //! Constructs the object with `stream`.
    explicit Printer(std::ostream *stream) noexcept :
        mStream{stream}
    {
    }

    //! Outputs the heading of the test run: the number of tests and test suites
    //! that are being run.
    auto beginRun(const Report &report) -> void
    {
        stream() << "Running " << report.tests << " tests from " << report.testSuites << " test suites...\n"
                 << separator() << "\n";
    }

    //! This method prints nothing but increases the indentation level.
    auto beginTest([[maybe_unused]] const Test *test) -> void
    {
        mIndentLevel++;
    }

    //! Prints the name of the test suite prepended with its source location.
    auto beginTestSuite(const TestSuite *testSuite) -> void
    {
        stream() << "---\n";

        if (hasLocation(testSuite))
        {
            stream() << gray(sourceLocationToString(testSuite->sourceLocation))
                     << testSuite->name << "\n";
        }

        mIndentLevel++;
    }

    //! Prints the summary of the test run in semi tabular form. The summary
    //! includes whether the run passed or failed, its total duration,
    //! number of tests run/passed/failed and number of expectations (assertions)
    //! run/passed/failed.
    auto endRun(const Report &report, const std::vector<TestSuite> &testSuites) -> void
    {
        const int width = static_cast<int>(std::to_string(std::max(report.tests, report.expectations)).size());
        const int passedWidth = static_cast<int>(std::to_string(std::max(report.failedTests, report.failedExpectations)).size());

        if (report.failures != 0)
        {
            stream() << separator() << "\n";
            printFailures(testSuites);
        }

        stream() << separator() << "\n"
                 << "Result      : " << (report.failedTests == 0 ? green("PASSED") : red("FAILED")) << '\n'
                 << "Duration    : " << std::chrono::duration_cast<std::chrono::milliseconds>(report.duration).count() << "ms\n"
                 << "Tests       : " << std::left << std::setw(width) << report.tests << " | "
                 << std::left << std::setw(passedWidth) << (report.tests - report.failedTests) << " passed | "
                 << report.failedTests << " failed\n"
                 << "Expectations: " << std::left << std::setw(width) << report.expectations << " | "
                 << std::left << std::setw(passedWidth) << (report.expectations - report.failedExpectations) << " passed | "
                 << report.failedExpectations << " failed\n";
    }

    //! Prints the information about just concluded test: whether
    //! it passed, its source location, name and duration. It also
    //! decreases the indent level.
    auto endTest(const Test *test) -> void
    {
        printTestResult(test);
        printTestFailures(test);
        mIndentLevel--;
    }

    //! Prints and empty line and decreses the indent level.
    auto endTestSuite([[maybe_unused]] const TestSuite *testSuite) -> void
    {
        stream() << '\n';
        mIndentLevel--;
    }

private:
    [[nodiscard]] static auto gray(const std::string &text) -> std::string
    {
        return Color::GRAY + text + Color::RESET;
    }

    [[nodiscard]] static auto green(const std::string &text) -> std::string
    {
        return Color::GREEN + text + Color::RESET;
    }

    [[nodiscard]] static auto hasLocation(const TestSuite *testSuite) -> bool
    {
        return strlen(testSuite->sourceLocation.file_name()) != strlen(std::source_location{}.file_name());
    }

    [[nodiscard]] auto indent() const -> std::string
    {
        return std::string(mIndentLevel * 2, ' ');
    }

    auto print(const std::string &prefix, const std::string &text) -> void
    {
        if (!text.empty())
        {
            stream() << indent() << prefix << ' ' << text << '\n';
        }
    }

    auto printFailures(const std::vector<TestSuite> &testSuites) -> void
    {
        for (const TestSuite &testSuite : testSuites)
        {
            for (const Test &test : testSuite.tests)
            {
                if (!test.failures.empty())
                {
                    printTestResult(&test);
                    printTestFailures(&test);
                }
            }
        }
    }

    auto printTestFailure(const Failure &failure) -> void
    {
        print(stringify("at ", yellow(sourceLocationToString(failure.sourceLocation))), failure.what);
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

    auto printTestResult(const Test *test) -> void
    {
        stream() << (test->failures.empty() ? green(" [ PASSED ] ") : red(" [ FAILED ] ")) << gray(sourceLocationToString(test->sourceLocation)) << test->name << " [" << std::chrono::duration_cast<std::chrono::milliseconds>(test->duration).count() << "ms]\n";
    }

    [[nodiscard]] static auto red(const std::string &text) -> std::string
    {
        return Color::RED + text + Color::RESET;
    }

    [[nodiscard]] static auto separator() -> std::string
    {
        constexpr size_t SEPARATOR_WIDTH = 75;
        return std::string(SEPARATOR_WIDTH, '=');
    }

    [[nodiscard]] static auto sourceLocationToString(const std::source_location &location) -> std::string
    {
        return stringify(std::filesystem::path{location.file_name()}.filename().string(), ':', location.line(), ':');
    }

    [[nodiscard]] auto stream() noexcept -> std::ostream &
    {
        return *mStream;
    }

    [[nodiscard]] static auto yellow(const std::string &text) -> std::string
    {
        return Color::YELLOW + text + Color::RESET;
    }

    std::ostream *mStream = nullptr;
    int mIndentLevel = 0;
};
}
