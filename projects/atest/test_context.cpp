#ifndef __clang__
export module atest:test_context;
import :test_suite;
#endif

namespace atest
{
//! \private
export class TestContext
{
    struct TestContextWrapper
    {
        TestContext *context = nullptr;
        std::atomic<int> instances{};
    };

public:
    TestContext() noexcept :
        parentContext{TestContext::current().context}
    {
        if (TestContext::current().instances.load() != 0)
        {
            TestContext::current().context = this;
        }

        ++TestContext::current().instances;
    }

    TestContext(const TestContext &other) = delete;
    TestContext(TestContext &&other) noexcept = delete;

    ~TestContext()
    {
        TestContext::current().context = this->parentContext;
        --TestContext::current().instances;
    }

    auto add_test_suite(const char *name,
                        auto(*body)()->void,
                        const std::source_location &sourceLocation) noexcept -> int
    {
        try
        {
            try
            {
                this->currentTestSuite = &this->testSuites.emplace_back(TestSuite{name, sourceLocation});
                body();
                this->currentTestSuite = &this->testSuites.front();
                return 0;
            }
            catch (const std::exception &exception)
            {
                std::cout << "ERROR: Exception thrown during registration of '"
                          << name
                          << "' test suite ("
                          << typeid(exception).name()
                          << "): " << exception.what()
                          << '\n';
            }
            catch (...)
            {
                std::cout << "ERROR: Unknown exception thrown during registration of '"
                          << name
                          << "' test suite.\n";
            }
        }
        catch (...)
        {
            // Suppress any further exceptions as this
            // function is usually run outside of main
            // and no exception can be caught. See
            // clang-tidy check: cert-err58-cpp.
        }

        return 1;
    }

    [[nodiscard]] static auto current() noexcept -> TestContextWrapper &
    {
        static TestContext globalContext{nullptr};
        static TestContextWrapper wrapper{.context = &globalContext};
        return wrapper;
    }

    [[nodiscard]] auto current_test() noexcept -> Test &
    {
        return *this->currentTest;
    }

    [[nodiscard]] auto current_test_suite() noexcept -> TestSuite &
    {
        return *this->currentTestSuite;
    }

    auto reset() -> void
    {
        for (TestSuite &testSuite : this->testSuites)
        {
            TestContext::reset_test_suites(testSuite);
        }
    }

    auto set_current_test(Test &test) noexcept -> void
    {
        this->currentTest = &test;
    }

    auto sort_test_suites() -> void
    {
        const auto sorter = [](const TestSuite &left, const TestSuite &right) {
            return std::string{left.sourceLocation.file_name()} < std::string{right.sourceLocation.file_name()};
        };

        std::sort(++this->testSuites.begin(), this->testSuites.end(), sorter);
    }

    [[nodiscard]] auto test_suites() noexcept -> std::vector<TestSuite> &
    {
        return this->testSuites;
    }

    auto operator=(const TestContext &other) -> TestContext & = delete;
    auto operator=(TestContext &&other) noexcept -> TestContext & = delete;

private:
    TestContext(TestContext *context) :
        parentContext{context}
    {
    }

    static auto reset_test(Test &test) -> void
    {
        test.expectations = 0;
        test.failedExpectations = 0;
        test.duration = std::chrono::microseconds::zero();
        test.failures.clear();
    }

    static auto reset_test_suites(TestSuite &testSuite) -> void
    {
        for (Test &test : testSuite.tests)
        {
            TestContext::reset_test(test);
        }
    }

    std::vector<TestSuite> testSuites = {TestSuite{"Global"}};
    TestSuite *currentTestSuite = &this->testSuites.front();
    Test *currentTest = nullptr;
    TestContext *parentContext = nullptr;
};

//! \private
export [[nodiscard]] auto test_context() -> TestContext &
{
    return *TestContext::current().context;
}
}
