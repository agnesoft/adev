#ifndef __clang__
module atest : test_context;
import : test_suite;
#endif

namespace atest
{
//! \private
class TestContext
{
public:
    TestContext() :
        parentContext{TestContext::current()}
    {
        TestContext::current() = this;
    }

    TestContext(TestContext *context) :
        parentContext{context}
    {
    }

    ~TestContext()
    {
        TestContext::current() = this->parentContext;
    }

    auto add_test_suite(const char *name,
                        auto (*body)()->void,
                        const std::source_location &sourceLocation) noexcept -> void
    {
        try
        {
            try
            {
                this->currentTestSuite = &this->testSuites.emplace_back(TestSuite{name, sourceLocation});
                body();
                this->currentTestSuite = &this->testSuites.front();
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
    }

    [[nodiscard]] static auto current() -> TestContext *&
    {
        static TestContext globalContext{nullptr};
        static TestContext *currentContext = &globalContext;
        return currentContext;
    }

    [[nodiscard]] auto current_test() noexcept -> Test &
    {
        return *this->currentTest;
    }

    [[nodiscard]] auto current_test_suite() noexcept -> TestSuite &
    {
        return *this->currentTestSuite;
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

private:
    std::vector<TestSuite> testSuites = {TestSuite{"Global"}};
    TestSuite *currentTestSuite = &this->testSuites.front();
    Test *currentTest = nullptr;
    TestContext *parentContext = nullptr;
};

[[nodiscard]] auto test_context() -> TestContext &
{
    return *TestContext::current();
}
}
