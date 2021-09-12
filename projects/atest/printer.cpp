#ifndef __clang__
module atest : printer;
import : test_suite;
import : report;
import : stringify;
#endif

namespace atest
{
//! \private
struct Color
{
    static constexpr char GRAY[] = "\033[1;30m";
    static constexpr char GREEN[] = "\033[1;32m";
    static constexpr char RED[] = "\033[1;31m";
    static constexpr char RESET[] = "\033[0m";
    static constexpr char YELLOW[] = "\033[1;33m";
};

//! \private
class Printer
{
public:
    explicit Printer(std::ostream &stream) noexcept :
        stream{stream}
    {
    }

    auto begin_run(const Report &report) -> void
    {
        this->stream << "Running " << report.tests << " tests from " << report.testSuites << " test suites...\n"
                     << Printer::separator() << "\n";
    }

    auto begin_test([[maybe_unused]] const Test &test) -> void
    {
        this->indentLevel++;
    }

    auto begin_test_suite(const TestSuite &testSuite) -> void
    {
        this->stream << "---\n";

        if (Printer::has_location(testSuite))
        {
            this->stream << Printer::gray(Printer::source_location_to_string(testSuite.sourceLocation))
                         << testSuite.name << "\n";
        }

        this->indentLevel++;
    }

    auto end_run(const Report &report, const std::vector<TestSuite> &testSuites) -> void
    {
        const int width = static_cast<int>(std::to_string(std::max(report.tests, report.expectations)).size());
        const int passedWidth = static_cast<int>(std::to_string(std::max(report.failedTests, report.failedExpectations)).size());

        if (report.failures != 0)
        {
            this->stream << Printer::separator() << "\n";
            this->print_failures(testSuites);
        }

        this->stream << Printer::separator() << "\n"
                     << "Result      : " << (report.failedTests == 0 ? green("PASSED") : red("FAILED")) << '\n'
                     << "Duration    : " << std::chrono::duration_cast<std::chrono::milliseconds>(report.duration).count() << "ms\n"
                     << "Tests       : " << std::left << std::setw(width) << report.tests << " | "
                     << std::left << std::setw(passedWidth) << (report.tests - report.failedTests) << " passed | "
                     << report.failedTests << " failed\n"
                     << "Expectations: " << std::left << std::setw(width) << report.expectations << " | "
                     << std::left << std::setw(passedWidth) << (report.expectations - report.failedExpectations) << " passed | "
                     << report.failedExpectations << " failed\n";
    }

    auto end_test(const Test &test) -> void
    {
        this->print_test_result(test);
        this->print_test_failures(test);
        this->indentLevel--;
    }

    auto end_test_suite([[maybe_unused]] const TestSuite &testSuite) -> void
    {
        this->stream << '\n';
        this->indentLevel--;
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

    [[nodiscard]] static auto has_location(const TestSuite &testSuite) -> bool
    {
        return ::strlen(testSuite.sourceLocation.file_name()) != ::strlen(std::source_location{}.file_name());
    }

    [[nodiscard]] auto indent() const -> std::string
    {
        return std::string(this->indentLevel * 2, ' ');
    }

    auto print(const std::string &prefix, const std::string &text) -> void
    {
        if (!text.empty())
        {
            this->stream << this->indent() << prefix << ' ' << text << '\n';
        }
    }

    auto print_failures(const Test &test) -> void
    {
        if (!test.failures.empty())
        {
            this->print_test_result(test);
            this->print_test_failures(test);
        }
    }

    auto print_failures(const TestSuite &testSuite) -> void
    {
        for (const Test &test : testSuite.tests)
        {
            this->print_failures(test);
        }
    }

    auto print_failures(const std::vector<TestSuite> &testSuites) -> void
    {
        for (const TestSuite &test_suite : testSuites)
        {
            this->print_failures(test_suite);
        }
    }

    auto print_test_failure(const Failure &failure) -> void
    {
        this->print(::atest::stringify("at ", Printer::yellow(Printer::source_location_to_string(failure.sourceLocation))), failure.what);
        this->print("  Expected: ", failure.expected);
        this->print("  Actual  : ", failure.actual);
    }

    auto print_test_failures(const Test &test) -> void
    {
        for (const Failure &failure : test.failures)
        {
            this->print_test_failure(failure);
        }
    }

    auto print_test_result(const Test &test) -> void
    {
        this->stream << (test.failures.empty() ? Printer::green(" [ PASSED ] ") : Printer::red(" [ FAILED ] "))
                     << Printer::gray(Printer::source_location_to_string(test.sourceLocation))
                     << test.name << " ["
                     << std::chrono::duration_cast<std::chrono::milliseconds>(test.duration).count()
                     << "ms]\n";
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

    [[nodiscard]] static auto source_location_to_string(const std::source_location &sourceLocation) -> std::string
    {
        return ::atest::stringify(std::filesystem::path{sourceLocation.file_name()}.filename().string(), ':', sourceLocation.line(), ':');
    }

    [[nodiscard]] static auto yellow(const std::string &text) -> std::string
    {
        return Color::YELLOW + text + Color::RESET;
    }

    std::ostream &stream;
    int indentLevel = 0;
};
}
