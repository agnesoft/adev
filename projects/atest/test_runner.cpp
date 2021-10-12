#ifndef __clang__
export module atest : test_runner;
import : failed_assertion;
import : test_context;
import : test_filter;
import : printer;
import : reporter;
#endif

namespace atest
{
//! The `TestRunner` provides test orchestration.
//!
//! The tests are created and registered using
//! test() and suite() functions and eventually
//! run with TestRunner::run(). The class takes
//! arguments from `main()` (currently unused) as
//! well as an optional output stream (`std::cout`
//! by default).
export class TestRunner
{
public:
    //! Constructs the object with `std::cout` as
    //! the output stream.
    TestRunner() :
        TestRunner{std::cout}
    {
    }

    //! Constructs the object with `stream` as the
    //! output stream.
    explicit TestRunner(std::ostream &stream) :
        printer{stream}
    {
    }

    //! Runs the tests. The test suites will be
    //! run in alphabetical order. Tests are
    //! always run in the order of declaration
    //! within a test suite. The global test suite
    //! is always run first. All test suites and
    //! tests are run sequentially (no
    //! parallelization). While the exceptions
    //! cannot be propagated from the tests
    //! themselves the orchestration code can
    //! still throw an exception in rare
    //! circumstances (out of memory etc.).
    //!
    //! Returns number of failures. `0` if all
    //! tests and expectations passed. You would
    //! typically set it as a return value of
    //! `main()`. Note that not only failed
    //! expectations are counted as failures but
    //! also unknown/unexpected exceptions are as
    //! well. I.e. it is possible to have all
    //! expectations passing but still getting
    //! non-0 amount of failures.
    [[nodiscard]] auto run(int argc, const char *const *argv) -> int
    {
        this->filter = TestFilter{argc, argv, this->printer.output_stream()};
        this->begin_run();
        this->run_test_suites();
        return this->end_run();
    }

private:
    auto begin_run() -> void
    {
        ::atest::test_context().reset();
        ::atest::test_context().sort_test_suites();
        const Stats stats = Reporter::stats(::atest::test_context().test_suites());
        this->printer.begin_run(stats);
    }

    [[nodiscard]] auto end_run() -> int
    {
        const Results results = Reporter::results(::atest::test_context().test_suites());
        this->printer.end_run(results, ::atest::test_context().test_suites());
        return static_cast<int>(results.failures);
    }

    auto run_test(Test &test) -> void
    {
        ::atest::test_context().set_current_test(test);
        this->printer.begin_test(test);
        TestRunner::run_test_body_measured(test);
        this->printer.end_test(test);
    }

    static auto run_test_body(Test &test) -> void
    {
        try
        {
            test.body();
        }
        catch ([[maybe_unused]] FailedAssertion &exception)
        {
            return;
        }
        catch (std::exception &exception)
        {
            test.failures.emplace_back(Failure{::atest::stringify("Unexpected exception thrown: ", exception.what()), "", "", test.sourceLocation});
        }
        catch (...)
        {
            test.failures.emplace_back(Failure{"Unexpected exception thrown", "", "", test.sourceLocation});
        }
    }

    static auto run_test_body_measured(Test &test) -> void
    {
        const auto start = std::chrono::steady_clock::now();
        TestRunner::run_test_body(test);
        const auto end = std::chrono::steady_clock::now();
        test.duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }

    auto run_tests(TestSuite &testSuite) -> void
    {
        for (Test &test : testSuite.tests)
        {
            if (this->filter.is_test_selected(test.name))
            {
                this->run_test(test);
            }
        }
    }

    auto run_test_suite(TestSuite &testSuite) -> void
    {
        if (!testSuite.tests.empty() && this->filter.is_suite_selected(testSuite.name))
        {
            this->printer.begin_test_suite(testSuite);
            this->run_tests(testSuite);
            this->printer.end_test_suite(testSuite);
        }
    }

    auto run_test_suites() -> void
    {
        for (TestSuite &testSuite : ::atest::test_context().test_suites())
        {
            this->run_test_suite(testSuite);
        }
    }

    const TestContext context;
    TestFilter filter{0, nullptr, std::cout};
    Printer printer;
};
}
