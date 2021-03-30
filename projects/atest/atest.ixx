module;

#ifdef _MSC_VER
#    include <typeinfo>
#endif

export module atest;

export import "astl.hpp";

namespace atest
{
struct Failure
{
    std::string what;
};

struct Test
{
    const char *description = "";
    auto (*testBody)() -> void = nullptr;
    int expectations = 0;
    std::vector<Failure> failures;
};

struct TestSuite
{
    const char *name = "";
    std::vector<Test> tests;
};

class TestRunner
{
public:
    ~TestRunner()
    {
        try
        {
            runTestSuites();
        }
        catch (...)
        {
            mFailed = true;
            std::cout << "Unexpected exception when running tests.\n";
        }

        std::exit(mFailed ? EXIT_FAILURE : EXIT_SUCCESS);
    }

    auto addTest(const char *name, auto (*testBody)()->void) -> void
    {
        mCurrentSuite->tests.emplace_back(Test{name, testBody, 0, {}});
    }

    auto beginSuite(const char *name) -> void
    {
        mCurrentSuite = &mTestSuites.emplace_back(TestSuite{name, {}});
    }

    [[nodiscard]] auto currentTest() const noexcept -> Test *
    {
        return mCurrentTest;
    }

    auto endSuite() -> void
    {
        mCurrentSuite = &mTestSuites.front();
    }

private:
    auto runTest(Test *test) -> void
    {
        std::cout << "  [" << test->description << "]... ";
        mCurrentTest = test;
        test->testBody();

        if (test->failures.empty())
        {
            std::cout << "SUCCESS\n";
        }
        else
        {
            mFailed = true;
            std::cout << "FAILED\n";

            for (const Failure &e : test->failures)
            {
                std::cout << "    " << e.what << '\n';
            }
        }
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
        std::cout << '"' << testSuite->name << "\":\n";
        runTests(testSuite);
        std::cout << '\n';
    }

    auto runTestSuites() -> void
    {
        for (TestSuite &testSuite : mTestSuites)
        {
            runTestSuite(&testSuite);
        }
    }

    std::vector<TestSuite> mTestSuites = std::vector<TestSuite>{{"Global", {}}};
    TestSuite *mCurrentSuite = &mTestSuites.front();
    Test *mCurrentTest = nullptr;
    bool mFailed = false;
};

[[nodiscard]] auto globalTestRunner() -> TestRunner *
{
    static TestRunner runner;
    return &runner;
}

export auto test(const char *description, auto (*testBody)()->void) -> void
{
    globalTestRunner()->addTest(description, testBody);
}

export auto suite(const char *name, auto (*suiteBody)()->void) -> int
{
    globalTestRunner()->beginSuite(name);
    suiteBody();
    globalTestRunner()->endSuite();
    return 0;
}

export template<typename T>
class ExpectBase
{
public:
    explicit ExpectBase(const T &expression) noexcept :
        mExpression{expression}
    {
        globalTestRunner()->currentTest()->expectations++;
    }

    auto toFail() noexcept
    {
        mExpectFailure = true;
    }

protected:
    [[nodiscard]] auto expectFailure() const noexcept -> bool
    {
        return mExpectFailure;
    }

    [[nodiscard]] auto expression() const noexcept -> const T &
    {
        return mExpression;
    }

    auto handleFailure(std::string message) -> void
    {
        if (!mExpectFailure)
        {
            fail(message);
        }
    }

    auto handleSuccess() -> void
    {
        if (mExpectFailure)
        {
            fail("Expected a failure but the test succeeded");
        }
    }

private:
    auto fail(std::string message) -> void
    {
        globalTestRunner()->currentTest()->expectations++;
        globalTestRunner()->currentTest()->failures.emplace_back(Failure{message});
    }

    const T &mExpression;
    bool mExpectFailure = false;
};

export template<typename T, typename V>
class ExpectToBe : public ExpectBase<T>
{
public:
    ExpectToBe(const T &expression, const V &value) :
        ExpectBase<T>{expression},
        mValue{value}
    {
    }

    ~ExpectToBe()
    {
        try
        {
            const auto left = evaluateExpression();

            if (left == mValue)
            {
                this->handleSuccess();
            }
            else
            {
                std::stringstream stream;
                stream << left << " != " << mValue;
                this->handleFailure(stream.str());
            }
        }
        catch (...)
        {
            handleUnknownException();
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

    auto handleUnknownException() -> void
    {
        std::stringstream stream;
        stream << "Unexpected exception thrown";

        try
        {
            throw;
        }
        catch (std::exception &e)
        {
            stream << ": " << e.what();
        }
        catch (...)
        {
        }

        this->handleFailure(stream.str());
    }

    const V &mValue;
};

export template<typename T, typename E, bool ValidateText>
requires std::invocable<T> class ExpectToThrow : public ExpectBase<T>
{
public:
    using ExpectBase<T>::ExpectBase;

    ExpectToThrow(const T &expression, std::string exceptionText) :
        ExpectBase<T>{expression},
        mExceptionText{std::move(exceptionText)}
    {
    }

    ~ExpectToThrow()
    {
        try
        {
            this->expression()();
            this->handleFailure("Expected the expression to throw but it did not.");
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
        if constexpr (ValidateText)
        {
            return mExceptionText == e.what();
        }
        else
        {
            return true;
        }
    }

    auto exceptionTextMismatch(std::string_view what) -> void
    {
        std::stringstream stream;
        stream << "Exception text mismatch.\n      "
               << "Expected: \"" << mExceptionText << "\"\n      "
               << "Actual  : \"" << what << "\"";
        this->handleFailure(stream.str());
    }

    auto exceptionTypeMismatch(const char *recievedExceptionName) -> void
    {
#ifdef _MSC_VER
        using ::type_info;
#endif

        std::stringstream stream;
        stream << "Expected exception of type '" << typeid(E).name() << "' but '" << recievedExceptionName << "' was thrown.";
        this->handleFailure(stream.str());
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
            exceptionTypeMismatch("...");
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
        if (exceptionTextMatches(e))
        {
            this->handleSuccess();
        }
        else
        {
            exceptionTextMismatch(e.what());
        }
    }

    std::string mExceptionText;
};

export template<typename T>
class Expect
{
public:
    Expect(const T &expression) noexcept :
        mExpression{expression}
    {
    }

    template<typename V>
    auto toBe(const V &value) -> ExpectToBe<T, V>
    {
        return ExpectToBe<T, V>{mExpression, value};
    }

    template<typename E>
    requires std::invocable<T> auto toThrow() -> ExpectToThrow<T, E, false>
    {
        return ExpectToThrow<T, E, false>{mExpression};
    }

    template<typename E>
    requires std::invocable<T> auto toThrow(const std::string &exceptionText) -> ExpectToThrow<T, E, true>
    {
        return ExpectToThrow<T, E, true>{mExpression, exceptionText};
    }

private:
    const T &mExpression;
};

export template<typename T>
[[nodiscard]] auto expect(const T &value) noexcept -> Expect<T>
{
    return Expect<T>{value};
}
}
