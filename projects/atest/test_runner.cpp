#ifdef _MSC_VER
export module atest : test_runner;

import : data;
import : reporter;
import : printer;
#endif

namespace atest
{
//! The `TestRunner` provides test orchestration.
//!
//! The tests are created and registered using test() and suite()
//! functions and eventually run by a `TestRunner`. The class
//! takes arguments from `main()` (currently unused)
//! as well as the output stream (std::cout by default). The tests are run
//! with run().
export class TestRunner
{
public:
    //! Constructs the object with main's arguments.
    TestRunner(int argc, char **argv) :
        TestRunner(argc, argv, &std::cout)
    {
    }

    //! Constructs the object with main's arguments and `stream`.
    TestRunner(int argc, char **argv, std::ostream *stream) :
        mPrinter{stream},
        mArgc{argc},
        mArgv{argv}
    {
    }

    //! Runs the tests. The test suites will be run in alphabetical
    //! order of the files they were declared in. If there are multiple
    //! tests suites in a file they will then be run in order of declaration.
    //! Tests are always run in the order of declaration. The global test
    //! suite is run first. All test suites and tests are run sequentially
    //! (no parallelization). While the exceptions cannot be propagated from
    //! the tests themselves the orchestration code can still throw an
    //! exception in rare circumstances (out of memory etc.).
    //!
    //! Returns number of failures. `0` if all tests and expectations passed.
    //! Usually set as a return value of `main()`. Note that not only failed
    //! expectations are counted as failures but also unknown/unexpected exeptions
    //! are as well. I.e. it is possible to have all expectations passing but
    //! still getting non-0 amount of failures.
    [[nodiscard]] auto run() -> int
    {
        sortTestSuites();
        mPrinter.beginRun(Reporter::generateStats(globalTests()->suites));
        runTestSuites();
        const auto report = Reporter::generateReport(globalTests()->suites);
        mPrinter.endRun(report);
        return static_cast<int>(report.failedExpectations);
    }

private:
    static auto sortTestSuites() -> void
    {
        std::sort(++globalTests()->suites.begin(), globalTests()->suites.end(), [](const TestSuite &left, const TestSuite &right) {
            return std::string{left.sourceLocation.file_name()} < std::string{right.sourceLocation.file_name()};
        });
    }

    auto runTest(Test *test) -> void
    {
        globalTests()->currentTest = test;
        mPrinter.beginTest(test);
        runTestBodyMeasured(test);
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
            test->failures.emplace_back(Failure{stringify("Unexpected exception thrown: ", e.what()), "", "", test->sourceLocation});
        }
        catch (...)
        {
            test->failures.emplace_back(Failure{"Unexpected exception thrown", "", "", test->sourceLocation});
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
};
}
