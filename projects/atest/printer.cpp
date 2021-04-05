#ifdef _MSC_VER
module atest : printer;

import : data;
import : stringify;
#endif

namespace atest
{
struct Color
{
    static constexpr char GRAY[] = "\033[1;30m";
    static constexpr char GREEN[] = "\033[1;32m";
    static constexpr char RED[] = "\033[1;31m";
    static constexpr char RESET[] = "\033[0m";
    static constexpr char YELLOW[] = "\033[1;33m";
};

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
                 << separator() << "\n";
    }

    auto beginTest([[maybe_unused]] const Test *test) -> void
    {
        mIndentLevel++;
    }

    auto beginTestSuite(const TestSuite *testSuite) -> void
    {
        stream() << "---\n"
                 << gray(sourceLocationToString(testSuite->sourceLocation))
                 << testSuite->name << "\n";
        mIndentLevel++;
    }

    auto endRun(const Report &report) -> void
    {
        const int width = static_cast<int>(std::to_string(std::max(report.tests, report.expectations)).size());
        const int passedWidth = static_cast<int>(std::to_string(std::max(report.failedTests, report.failedExpectations)).size());

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

    auto endTest(const Test *test) -> void
    {
        printTestResult(test);
        printTestFailures(test);
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
    [[nodiscard]] static auto gray(const std::string &text) -> std::string
    {
        return Color::GRAY + text + Color::RESET;
    }

    [[nodiscard]] static auto green(const std::string &text) -> std::string
    {
        return Color::GREEN + text + Color::RESET;
    }

    [[nodiscard]] auto indent() const -> std::string
    {
        return std::string(mIndentLevel * 2, ' ');
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

    [[nodiscard]] static auto sourceLocationToString(const source_location<> &location) -> std::string
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
