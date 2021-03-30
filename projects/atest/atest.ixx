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

    auto currentTest() const noexcept -> Test *
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

auto globalTestRunner() -> TestRunner *
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

export template<typename T, typename V>
class ExpectToBe
{
public:
    ExpectToBe(const T &expression, const V &value) :
        mExpression{expression},
        mValue{value}
    {
        globalTestRunner()->currentTest()->expectations++;
    }

    ~ExpectToBe()
    {
        try
        {
            const auto left = evaluateExpression<V>();

            if (left == mValue)
            {
                handleSuccess();
            }
            else
            {
                std::stringstream stream;
                stream << left << " != " << mValue;
                handleFailure(stream.str());
            }
        }
        catch (...)
        {
            handleException();
        }
    }

    auto toFail() noexcept
    {
        mExpectFailure = true;
    }

private:
    template<typename V>
    [[nodiscard]] auto evaluateExpression() const -> V
    {
        if constexpr (std::is_invocable<T>::value)
        {
            return mExpression();
        }
        else
        {
            return mExpression;
        }
    }

    auto fail(std::string message) -> void
    {
        globalTestRunner()->currentTest()->expectations++;
        globalTestRunner()->currentTest()->failures.emplace_back(Failure{message});
    }

    auto handleException() -> void
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

        handleFailure(stream.str());
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

    const T &mExpression;
    const V &mValue;
    bool mExpectFailure = false;
};

export template<typename T, typename E>
class ExpectToThrow
{
public:
    ExpectToThrow(const T &expression, std::string_view exceptionText) :
        mExpression{expression},
        mExceptionText{exceptionText}
    {
        globalTestRunner()->currentTest()->expectations++;
    }

    ~ExpectToThrow()
    {
#ifdef _MSC_VER
        using ::type_info;
#endif

        try
        {
            mExpression();
            handleFailure("Expected the expression to throw but it did not.");
        }
        catch (E &e)
        {
            if (typeid(E) != typeid(e))
            {
                std::stringstream stream;
                stream << "Expected exception of type '" << typeid(E).name() << "' but '" << typeid(e).name() << "' was thrown.";
                handleFailure(stream.str());
            }
            else if (!mExceptionText.empty() && mExceptionText != e.what())
            {
                std::stringstream stream;
                stream << "Exception text mismatch.\n      "
                       << "Expected: \"" << mExceptionText << "\"\n      "
                       << "Actual  : \"" << e.what() << "\"";
                handleFailure(stream.str());
            }
            else
            {
                handleSuccess();
            }
        }
        catch (...)
        {
            try
            {
                throw;
            }
            catch (std::exception &e)
            {
                std::stringstream stream;
                stream << "Expected exception of type '" << typeid(E).name() << "' but '" << typeid(e).name() << "' was thrown.";
                handleFailure(stream.str());
            }
            catch (...)
            {
                std::stringstream stream;
                stream << "Expected exception of type '" << typeid(E).name() << "' but different exception was thrown.";
                handleFailure(stream.str());
            }
        }
    }

    auto toFail() noexcept
    {
        mExpectFailure = true;
    }

private:
    auto fail(std::string message) -> void
    {
        globalTestRunner()->currentTest()->expectations++;
        globalTestRunner()->currentTest()->failures.emplace_back(Failure{message});
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

    const T &mExpression;
    std::string_view mExceptionText;
    bool mExpectFailure = false;
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
    auto toThrow(const std::string &exceptionText) -> ExpectToThrow<T, E>
    {
        return ExpectToThrow<T, E>{mExpression, exceptionText};
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
