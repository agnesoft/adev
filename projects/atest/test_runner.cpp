#ifndef __clang__
export module atest : test_runner;
import : failed_assertion;
import : global_tests;
import : printer;
import : reporter;
#endif

namespace atest
{
//! The `TestRunner` provides test orchestration.
//!
//! The tests are created and registered using
//! test() and suite() functions and eventually
//! run by a `TestRunner`. The class takes
//! arguments from `main()` (currently unused) as
//! well as the output stream (std::cout by
//! default). The tests are run with run().
export class TestRunner
{
public:
    //! Constructs the object with main's
    //! arguments.
    TestRunner(int argc, char *argv[]) :
        TestRunner(argc, argv, std::cout)
    {
    }

    //! Constructs the object with main's
    //! arguments and `stream`.
    TestRunner(int argc, char *argv[], std::ostream &stream) :
        printer{stream},
        argc{argc},
        argv{argv}
    {
    }

    //! Runs the tests. The test suites will be
    //! run in alphabetical order of the files
    //! they were declared in. If there are
    //! multiple tests suites in a file they will
    //! then be run in order of declaration. Tests
    //! are always run in the order of
    //! declaration. The global test suite is run
    //! first. All test suites and tests are run
    //! sequentially (no parallelization). While
    //! the exceptions cannot be propagated from
    //! the tests themselves the orchestration
    //! code can still throw an exception in rare
    //! circumstances (out of memory etc.).
    //!
    //! Returns number of failures. `0` if all
    //! tests and expectations passed. Usually set
    //! as a return value of `main()`. Note that
    //! not only failed expectations are counted
    //! as failures but also unknown/unexpected
    //! exceptions are as well. I.e. it is
    //! possible to have all expectations passing
    //! but still getting non-0 amount of
    //! failures.
    [[nodiscard]] auto run() -> int
    {
        TestRunner::sort_global_test_suites();
        this->printer.begin_run(Reporter::generate_stats(::atest::global_tests().suites));
        this->run_test_suites();

        const auto report = Reporter::generate_report(::atest::global_tests().suites);
        this->printer.end_run(report, ::atest::global_tests().suites);
        return static_cast<int>(report.failures);
    }

private:
    auto run_test(Test &test) -> void
    {
        ::atest::global_tests().currentTest = &test;
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
            this->run_test(test);
        }
    }

    auto run_test_suite(TestSuite &testSuite) -> void
    {
        if (!testSuite.tests.empty())
        {
            this->printer.begin_test_suite(testSuite);
            this->run_tests(testSuite);
            this->printer.end_test_suite(testSuite);
        }
    }

    auto run_test_suites() -> void
    {
        for (TestSuite &testSuite : ::atest::global_tests().suites)
        {
            this->run_test_suite(testSuite);
        }
    }

    static auto sort_global_test_suites() -> void
    {
        GlobalTests &tests = ::atest::global_tests();
        std::sort(++tests.suites.begin(),
                  tests.suites.end(),
                  [](const TestSuite &left, const TestSuite &right) {
                      return std::string{left.sourceLocation.file_name()} < std::string{right.sourceLocation.file_name()};
                  });
    }

    Printer printer;
    [[maybe_unused]] int argc = 0;
    [[maybe_unused]] char **argv = nullptr;
};
}
