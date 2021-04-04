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
export template<typename T = int>
class source_location
{
public:
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

struct Tests
{
    std::vector<TestSuite> suites = {TestSuite{"Global", source_location<>::current()}};
    TestSuite *currentTestSuite = &suites.front();
    Test *currentTest = nullptr;
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

[[nodiscard]] auto globalTests() -> Tests *
{
    static Tests tests;
    return &tests;
}

export template<typename T>
concept stringifiable = requires(const T &type)
{
    {std::ostringstream{} << type};
};

export template<typename T>
concept iterable = requires(const T &type)
{
    {type.begin()};
    {type.end()};
};

export template<typename T>
requires(!stringifiable<T> && iterable<T>) auto operator<<(std::ostream &stream, const T &container) -> std::ostream &
{
    stream << '{';

    for (auto it = container.begin(); it != container.end();)
    {
        stream << (*it);

        if (++it != container.end())
        {
            stream << ", ";
        }
    }

    stream << '}';
    return stream;
}

template<typename T, typename... Values>
auto stringifyImpl(std::stringstream &stream, const T &value, const Values &...values) -> void
{
    stream << value; //NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

    if constexpr (sizeof...(Values) > 0)
    {
        stringifyImpl(stream, values...);
    }
}

export template<typename... T>
[[nodiscard]] auto stringify(const T &...values) -> std::string
{
    std::stringstream stream;
    stringifyImpl(stream, values...);
    return stream.str();
}

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

        stream()
            << separator() << "\n"
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

    [[nodiscard]] auto testWidth() const noexcept -> size_t
    {
        return mTestWidth;
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

class Reporter
{
public:
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

export class TestRunner
{
public:
    TestRunner(int argc, char *argv[]) :
        TestRunner(argc, argv, &std::cout)
    {
    }

    TestRunner(int argc, char *argv[], std::ostream *stream) :
        mPrinter{stream},
        mArgc{argc},
        mArgv{static_cast<char **>(argv)}
    {
    }

    auto run() -> int
    {
        mPrinter.beginRun(Reporter::generateStats(globalTests()->suites));
        runTestSuites();
        mPrinter.endRun(Reporter::generateReport(globalTests()->suites));
        return mFailed ? 1 : 0;
    }

private:
    auto runTest(Test *test) -> void
    {
        globalTests()->currentTest = test;
        mPrinter.beginTest(test);
        runTestBodyMeasured(test);
        mFailed = mFailed || !test->failures.empty();
        mPrinter.endTest(test);
    }

    static auto runTestBody(Test *test) -> void
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

    static auto runTestBodyMeasured(Test *test) -> void
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
        for (TestSuite &testSuite : globalTests()->suites)
        {
            runTestSuite(&testSuite);
        }
    }

    Printer mPrinter;
    [[maybe_unused]] int mArgc = 0;
    [[maybe_unused]] char **mArgv = nullptr;
    bool mFailed = false;
};

export template<typename T>
class ExpectBase
{
public:
    explicit ExpectBase(const T &expression, const source_location<> &sourceLocation) noexcept :
        mExpression{expression},
        mSourceLocation{sourceLocation}
    {
        globalTests()->currentTest->expectations++;
    }

    auto toFail() noexcept -> void
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
        globalTests()->currentTest->failedExpectations++;
        globalTests()->currentTest->failures.emplace_back(std::move(failure));
    }

    const T &mExpression;
    bool mExpectFailure = false;
    source_location<> mSourceLocation;
};

export template<typename T, typename V, typename Matcher>
class ExpectToMatch : public ExpectBase<T>
{
public:
    ExpectToMatch(const T &expression, const V &value, const source_location<> &sourceLocation) :
        ExpectBase<T>{expression, sourceLocation},
        mValue{value}
    {
    }

    ExpectToMatch(const ExpectToMatch &other) = default;
    ExpectToMatch(ExpectToMatch &&other) noexcept = default;

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

    auto operator=(const ExpectToMatch &other) -> ExpectToMatch & = default;
    auto operator=(ExpectToMatch &&other) noexcept -> ExpectToMatch & = default;

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

    ExpectToThrow(const T &expression, std::string exceptionText, const source_location<> &sourceLocation) :
        ExpectBase<T>{expression, sourceLocation},
        mExceptionText{std::move(exceptionText)}
    {
    }

    ExpectToThrow(const ExpectToThrow &other) = default;
    ExpectToThrow(ExpectToThrow &&other) noexcept = default;

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

    auto operator=(const ExpectToThrow &other) -> ExpectToThrow & = default;
    auto operator=(ExpectToThrow &&other) noexcept -> ExpectToThrow & = default;

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
    [[nodiscard]] static auto describe() -> std::string
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
    Expect(const T &expression, const source_location<> &sourceLocation) noexcept :
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
auto suite(const char *name, auto (*suiteBody)()->void, const source_location<> &sourceLocation = source_location<>::current()) noexcept -> int
{
    try
    {
        try
        {
            Tests *tests = globalTests();
            tests->currentTestSuite = &tests->suites.emplace_back(TestSuite{name, sourceLocation});
            suiteBody();
            tests->currentTestSuite = &tests->suites.front();
            return 0;
        }
        catch (std::exception &e)
        {
            std::cout << "Exception thrown during registration of '" << name << "' test suite:\n  " << e.what();
        }
        catch (...)
        {
            std::cout << "Unknown exception thrown during registration of '" << name << "' test suite.";
        }
    }
    catch (...)
    {
        //Supress any further exceptions as this function is
        //usually run outside of main and no exception can
        //be caught. See clang-tidy check: cert-err58-cpp.
    }

    return 1;
}

export template<typename T = int>
auto test(const char *name, auto (*testBody)()->void, const source_location<> &sourceLocation = source_location<>::current()) -> void
{
    globalTests()->currentTestSuite->tests.emplace_back(Test{name, testBody, sourceLocation});
}

export template<typename T>
[[nodiscard]] auto expect(const T &value, const source_location<> &sourceLocation = source_location<>::current()) noexcept -> Expect<T>
{
    return Expect<T>{value, sourceLocation};
}
}
