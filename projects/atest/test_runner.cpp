export module atest : test_runner;

import : data;
import : reporter;
import : printer;

namespace atest
{
export class TestRunner
{
public:
    TestRunner(int argc, char **argv) :
        TestRunner(argc, argv, &std::cout)
    {
    }

    TestRunner(int argc, char **argv, std::ostream *stream) :
        mPrinter{stream},
        mArgc{argc},
        mArgv{argv}
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
    bool mFailed = false;
};
}
