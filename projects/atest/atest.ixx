module;

#ifdef _MSC_VER
#    include <typeinfo>
#endif

export module atest;

export import "astl.hpp";

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

struct Failure
{
    std::string what;
    std::string expected;
    std::string actual;
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
        mCurrentSuite->tests.emplace_back(Test{name, testBody});
    }

    auto beginSuite(const char *name) -> void
    {
        mCurrentSuite = &mTestSuites.emplace_back(TestSuite{name});
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
                if (!e.what.empty())
                {
                    std::cout << "    " << e.what << '\n';
                }

                if (!e.expected.empty())
                {
                    std::cout << "      Expected: " << e.expected << '\n'
                              << "      Actual  : " << e.actual << '\n';
                }
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

    std::vector<TestSuite> mTestSuites = std::vector<TestSuite>{{"Global"}};
    TestSuite *mCurrentSuite = &mTestSuites.front();
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
    auto fail(Failure failure) -> void
    {
        globalTestRunner()->currentTest()->expectations++;
        globalTestRunner()->currentTest()->failures.emplace_back(std::move(failure));
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
#ifdef _MSC_VER
        using ::type_info;
#endif

        try
        {
            const auto left = evaluateExpression();

            if (left == mValue)
            {
                this->handleSuccess();
            }
            else
            {
                this->handleFailure(Failure{"Values are not equal", stringify(left), stringify(mValue)});
            }
        }
        catch (std::exception &e)
        {
            this->handleFailure(Failure{"Unexpected exception thrown", stringify(mValue), stringify(typeid(e).name(), " '", e.what(), '\'')});
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

    ExpectToThrow(const T &expression, std::string exceptionText) :
        ExpectBase<T>{expression},
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
        if constexpr (ValidateText)
        {
            return mExceptionText == e.what();
        }
        else
        {
            return true;
        }
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

export auto suite(const char *name, auto (*suiteBody)()->void) -> int
{
    globalTestRunner()->beginSuite(name);
    suiteBody();
    globalTestRunner()->endSuite();
    return 0;
}

export auto test(const char *description, auto (*testBody)()->void) -> void
{
    globalTestRunner()->addTest(description, testBody);
}

export template<typename T>
[[nodiscard]] auto expect(const T &value) noexcept -> Expect<T>
{
    return Expect<T>{value};
}
}
