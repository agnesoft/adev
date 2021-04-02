module;

#ifdef _MSC_VER
#    include <typeinfo>
#endif

export module atest;

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 5106)
#endif

export import "astl.hpp";

#ifdef _MSC_VER
#    pragma warning(pop)
#endif

namespace atest
{
template<typename T, typename... Values>
auto stringifyImpl(std::stringstream &stream, const T &value, const Values &...values) -> void
{
    stream << value;

    if constexpr (sizeof...(Values) > 0)
    {
        stringifyImpl(stream, values...);
    }
}

template<typename... T>
[[nodiscard]] auto stringify(const T &...values) -> std::string
{
    std::stringstream stream;
    stringifyImpl(stream, values...);
    return stream.str();
}

export template<typename T = int>
class source_location
{
public:
    source_location() = default;

    source_location(const source_location &other)
    {
        *this = other;
    }

    source_location(const source_location &&other) noexcept
    {
        *this = std::move(other);
    }

    ~source_location() = default;

    [[nodiscard]] static auto current(const char *file = __builtin_FILE(), int line = __builtin_LINE()) noexcept -> source_location
    {
        source_location sourceLocation;
        sourceLocation.mFile = file;
        sourceLocation.mLine = line;
        return sourceLocation;
    }

    [[nodiscard]] constexpr auto file_name() const noexcept -> const char *
    {
        return mFile;
    }

    [[nodiscard]] constexpr auto line() const noexcept -> int
    {
        return mLine;
    }

    auto operator=(const source_location &other) -> source_location &
    {
        if (this != &other)
        {
            mFile = other.mFile;
            mLine = other.mLine;
        }

        return *this;
    }

    auto operator=(source_location &&other) noexcept -> source_location &
    {
        if (this != &other)
        {
            mFile = other.mFile;
            mLine = other.mLine;
            other.mFile = nullptr;
            other.mline = 0;
        }

        return *this;
    }

private:
    const char *mFile = "unknown";
    int mLine = -1;
};

struct Failure
{
    std::string what;
    std::string expected;
    std::string actual;
    source_location<> sourceLocation;
};

struct Test
{
    std::string name;
    auto (*testBody)() -> void = nullptr;
    source_location<> sourceLocation;
    size_t expectations = 0;
    size_t failedExpectations = 0;
    std::chrono::microseconds duration = std::chrono::microseconds::zero();
    std::vector<Failure> failures;
};

struct TestSuite
{
    std::string name;
    source_location<> sourceLocation;
    std::vector<Test> tests;
};

struct Report
{
    size_t testSuites = 0;
    size_t tests = 0;
    size_t failedTests = 0;
    size_t expectations = 0;
    size_t failedExpectations = 0;
    std::chrono::microseconds duration = std::chrono::microseconds::zero();
};

class Printer
{
public:
    Printer() :
        mStream{&std::cout}
    {
    }

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
        stream() << indent() << std::left << std::setw(mTestWidth + 6) << stringify(sourceLocationToString(test->sourceLocation), " \"", test->name, "\"...");
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
        const size_t width = std::to_string(std::max(report.tests, report.expectations)).size();
        const size_t passedWidth = std::to_string(std::max(report.failedTests, report.failedExpectations)).size();

        stream()
            << separator() << "\n"
            << "Result      : " << (report.failedTests == 0 ? "PASSED" : "FAILED") << '\n'
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

    auto print(const std::string &text) -> void
    {
        print("", text);
    }

    auto print(const std::string &prefix, const std::string &text) -> void
    {
        if (!text.empty())
        {
            stream() << indent() << prefix << ' ' << text << '\n';
        }
    }

private:
    [[nodiscard]] auto sourceLocationToString(source_location<> location) const -> std::string
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

    [[nodiscard]] auto separator() const -> std::string
    {
        return std::string(75, '=');
    }

    [[nodiscard]] auto stream() noexcept -> std::ostream &
    {
        return *mStream;
    }

    [[nodiscard]] auto testWidth() const noexcept -> size_t
    {
        return mTestWidth;
    }

    [[nodiscard]] auto testNamesWidth(const TestSuite *testSuite) const -> size_t
    {
        size_t width = 0;

        for (const Test &test : testSuite->tests)
        {
            if (width < test.name.size())
            {
                width = test.name.size();
            }
        }

        return width;
    }

    [[nodiscard]] auto testsWidth(const TestSuite *testSuite) const -> size_t
    {
        return sourceLocationToString(testSuite->tests.back().sourceLocation).size() + testNamesWidth(testSuite);
    }

    std::ostream *mStream = nullptr;
    int mIndentLevel = 0;
    size_t mTestWidth = 0;
};

class Reporter
{
public:
    [[nodiscard]] auto generateReport(const std::vector<TestSuite> &testSuites) const -> Report
    {
        Report report;
        report.testSuites = testSuitesCount(testSuites);

        for (const TestSuite &testSuite : testSuites)
        {
            reportTestSuite(&report, testSuite);
        }

        return report;
    }

    [[nodiscard]] auto generateStats(const std::vector<TestSuite> &testSuites) const -> Report
    {
        Report report;
        report.testSuites = testSuitesCount(testSuites);
        report.tests = std::accumulate(testSuites.begin(), testSuites.end(), size_t{0}, [](size_t count, const TestSuite &testSuite) { return count + testSuite.tests.size(); });
        return report;
    }

private:
    auto reportTest(Report *report, const Test &test) const -> void
    {
        report->expectations += test.expectations;
        report->duration += test.duration;

        if (!test.failures.empty())
        {
            report->failedTests++;
            report->failedExpectations += test.failedExpectations;
        }
    }

    auto reportTestSuite(Report *report, const TestSuite &testSuite) const -> void
    {
        report->tests += testSuite.tests.size();

        for (const Test &test : testSuite.tests)
        {
            reportTest(report, test);
        }
    }

    [[nodiscard]] auto testSuitesCount(const std::vector<TestSuite> &testSuites) const -> size_t
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

class TestRunner
{
public:
    ~TestRunner()
    {
        try
        {
            mPrinter.beginRun(Reporter{}.generateStats(mTestSuites));
            runTestSuites();
        }
        catch (...)
        {
            mFailed = true;
            mPrinter.print("Unexpected exception thrown when running tests.");
        }

        mPrinter.endRun(Reporter{}.generateReport(mTestSuites));
        std::exit(mFailed ? EXIT_FAILURE : EXIT_SUCCESS);
    }

    auto addTest(const char *name, auto (*testBody)()->void, source_location<> sourceLocation) -> void
    {
        mCurrentTestSuite->tests.emplace_back(Test{name, testBody, sourceLocation});
    }

    auto beginRecordTests(const char *testSuiteName, source_location<> sourceLocation) -> void
    {
        mCurrentTestSuite = &mTestSuites.emplace_back(TestSuite{testSuiteName, sourceLocation});
    }

    [[nodiscard]] auto currentTest() const noexcept -> Test *
    {
        return mCurrentTest;
    }

    auto setPrinterStream(std::ostream *stream) -> void
    {
        mPrinter = Printer{stream};
    }

    auto stopRecordTests() -> void
    {
        mCurrentTestSuite = &mTestSuites.front();
    }

private:
    auto runTest(Test *test) -> void
    {
        mCurrentTest = test;
        mPrinter.beginTest(test);
        runTestBodyMeasured(test);
        mFailed = test->failures.empty();
        mPrinter.endTest(test);
    }

    auto runTestBody(Test *test) -> void
    {
        try
        {
            test->testBody();
        }
        catch (std::exception &e)
        {
            test->failures.emplace_back(Failure{stringify("Unexpected exception thrown: ", e.what())});
        }
        catch (...)
        {
            test->failures.emplace_back(Failure{"Unexpected exception thrown"});
        }
    }

    auto runTestBodyMeasured(Test *test) -> void
    {
        const auto start = std::chrono::steady_clock::now();
        runTestBody(test);
        const auto end = std::chrono::steady_clock::now();
        test->duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }

    auto runTests(TestSuite *testSuite) -> void
    {
        for (Test &test : testSuite->tests)
        {
            runTest(&test);
        }
    }

    auto runTestSuite(TestSuite *testSuite) -> void
    {
        if (!testSuite->tests.empty())
        {
            mPrinter.beginTestSuite(testSuite);
            runTests(testSuite);
            mPrinter.endTestSuite(testSuite);
        }
    }

    auto runTestSuites() -> void
    {
        for (TestSuite &testSuite : mTestSuites)
        {
            runTestSuite(&testSuite);
        }
    }

    Printer mPrinter;
    std::vector<TestSuite> mTestSuites = std::vector<TestSuite>{{"Global", source_location<>::current()}};
    TestSuite *mCurrentTestSuite = &mTestSuites.front();
    Test *mCurrentTest = nullptr;
    bool mFailed = false;
};

[[nodiscard]] auto globalTestRunner() -> TestRunner *
{
    static TestRunner runner;
    return &runner;
}

export template<typename T>
class ExpectBase
{
public:
    explicit ExpectBase(const T &expression, source_location<> sourceLocation) noexcept :
        mExpression{expression},
        mSourceLocation{sourceLocation}
    {
        globalTestRunner()->currentTest()->expectations++;
    }

    auto toFail() noexcept
    {
        mExpectFailure = true;
    }

protected:
    [[nodiscard]] auto expression() const noexcept -> const T &
    {
        return mExpression;
    }

    auto handleFailure(Failure &&failure) -> void
    {
        if (!mExpectFailure)
        {
            fail(std::move(failure));
        }
    }

    auto handleSuccess() -> void
    {
        if (mExpectFailure)
        {
            fail(Failure{"Expected a failure but the test succeeded."});
        }
    }

private:
    auto fail(Failure &&failure) -> void
    {
        failure.sourceLocation = mSourceLocation;
        globalTestRunner()->currentTest()->failedExpectations++;
        globalTestRunner()->currentTest()->failures.emplace_back(std::move(failure));
    }

    const T &mExpression;
    bool mExpectFailure = false;
    source_location<> mSourceLocation;
};

export template<typename T, typename V, typename Matcher>
class ExpectToMatch : public ExpectBase<T>
{
public:
    ExpectToMatch(const T &expression, const V &value, source_location<> sourceLocation) :
        ExpectBase<T>{expression, sourceLocation},
        mValue{value}
    {
    }

    ~ExpectToMatch()
    {
#ifdef _MSC_VER
        using ::type_info;
#endif

        try
        {
            const auto left = evaluateExpression();
            Matcher matcher;

            if (matcher(left, mValue))
            {
                this->handleSuccess();
            }
            else
            {
                this->handleFailure(Failure{matcher.describe(), matcher.expected(left, mValue), matcher.actual(left, mValue)});
            }
        }
        catch (std::exception &e)
        {
            this->handleFailure(Failure{stringify("Unexpected exception thrown (", typeid(e).name(), "): '", e.what(), '\'')});
        }
        catch (...)
        {
            this->handleFailure(Failure{"Unexpected exception thrown"});
        }
    }

private:
    [[nodiscard]] auto evaluateExpression() const -> V
    {
        if constexpr (std::is_invocable<T>::value)
        {
            return this->expression()();
        }
        else
        {
            return this->expression();
        }
    }

    const V &mValue;
};

export template<typename T, typename E, bool ValidateText>
requires std::invocable<T> class ExpectToThrow : public ExpectBase<T>
{
public:
    using ExpectBase<T>::ExpectBase;

    ExpectToThrow(const T &expression, std::string exceptionText, source_location<> sourceLocation) :
        ExpectBase<T>{expression, sourceLocation},
        mExceptionText{std::move(exceptionText)}
    {
    }

    ~ExpectToThrow()
    {
#ifdef _MSC_VER
        using ::type_info;
#endif

        try
        {
            this->expression()();
            this->handleFailure(Failure{"No exception thrown", stringify(typeid(E).name(), " '", mExceptionText, '\''), ""});
        }
        catch (E &e)
        {
            validateException(e);
        }
        catch (...)
        {
            handleUnknownException();
        }
    }

private:
    [[nodiscard]] auto exceptionTextMatches(E &e) -> bool
    {
        return mExceptionText == e.what();
    }

    auto exceptionTextMismatch(std::string what) -> void
    {
        this->handleFailure(Failure{"Exception text mismatch", mExceptionText, std::move(what)});
    }

    auto exceptionTypeMismatch(std::string recievedExceptionName) -> void
    {
#ifdef _MSC_VER
        using ::type_info;
#endif

        this->handleFailure(Failure{"Exception type mismatch", typeid(E).name(), std::move(recievedExceptionName)});
    }

    auto handleUnknownException()
    {
        try
        {
            throw;
        }
        catch (std::exception &e)
        {
#ifdef _MSC_VER
            using ::type_info;
#endif
            exceptionTypeMismatch(typeid(e).name());
        }
        catch (...)
        {
            exceptionTypeMismatch("unknown exception");
        }
    }

    auto validateException(E &e) -> void
    {
#ifdef _MSC_VER
        using ::type_info;
#endif
        if (typeid(E) == typeid(e))
        {
            validateExceptionText(e);
        }
        else
        {
            exceptionTypeMismatch(typeid(e).name());
        }
    }

    auto validateExceptionText(E &e) -> void
    {
        if constexpr (ValidateText)
        {
            if (exceptionTextMatches(e))
            {
                this->handleSuccess();
            }
            else
            {
                exceptionTextMismatch(e.what());
            }
        }
        else
        {
            this->handleSuccess();
        }
    }

    std::string mExceptionText;
};

export class MatcherBase
{
public:
    [[nodiscard]] auto describe() const -> std::string
    {
        return "Values are not equal.";
    }

    template<typename T, typename V>
    [[nodiscard]] auto expected(const T &left, [[maybe_unused]] const V &right) const -> std::string
    {
        return stringify(left);
    }

    template<typename T, typename V>
    [[nodiscard]] auto actual([[maybe_unused]] const T &left, const V &right) const -> std::string
    {
        return stringify(right);
    }
};

export class Matcher : public MatcherBase
{
public:
    template<typename T, typename V>
    [[nodiscard]] auto operator()(const T &left, const V &right) const -> bool
    {
        return left == right;
    }
};

export template<typename T>
class Expect
{
public:
    Expect(const T &expression, source_location<> sourceLocation) noexcept :
        mExpression{expression},
        mSourceLocation{sourceLocation}
    {
    }

    template<typename V>
    auto toBe(const V &value) -> ExpectToMatch<T, V, Matcher>
    {
        return ExpectToMatch<T, V, Matcher>{mExpression, value, mSourceLocation};
    }

    template<typename M, typename V>
    auto toMatch(const V &value) -> ExpectToMatch<T, V, M>
    {
        return ExpectToMatch<T, V, M>{mExpression, value, mSourceLocation};
    }

    template<typename E>
    requires std::invocable<T> auto toThrow() -> ExpectToThrow<T, E, false>
    {
        return ExpectToThrow<T, E, false>{mExpression, mSourceLocation};
    }

    template<typename E>
    requires std::invocable<T> auto toThrow(const std::string &exceptionText) -> ExpectToThrow<T, E, true>
    {
        return ExpectToThrow<T, E, true>{mExpression, exceptionText, mSourceLocation};
    }

private:
    const T &mExpression;
    source_location<> mSourceLocation;
};

export template<typename T = int>
auto suite(const char *name, auto (*suiteBody)()->void, source_location<> sourceLocation = source_location<>::current()) -> int
{
    globalTestRunner()->beginRecordTests(name, sourceLocation);
    suiteBody();
    globalTestRunner()->stopRecordTests();
    return 0;
}

export template<typename T = int>
auto test(const char *name, auto (*testBody)()->void, source_location<> sourceLocation = source_location<>::current()) -> void
{
    globalTestRunner()->addTest(name, testBody, sourceLocation);
}

export template<typename T>
[[nodiscard]] auto expect(const T &value, source_location<> sourceLocation = source_location<>::current()) noexcept -> Expect<T>
{
    return Expect<T>{value, sourceLocation};
}

export auto setPrinterStream(std::ostream *stream) -> void
{
    globalTestRunner()->setPrinterStream(stream);
}
}
