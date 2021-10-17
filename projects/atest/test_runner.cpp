#ifndef __clang__
export module atest : test_runner;
import : failed_assertion;
import : test_context;
import : test_filter;
import : printer;
import : reporter;
import : filters;
import acommandline;
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
    //!
    //! If the user requested help via `-?`
    //! command line argument the test run will be
    //! skipped and the help is displayed instead.
    //!
    //! If the user requests listing via --list or
    //! -l the test suites and tests (possibly
    //! filtered using other command line options)
    //! are only listed but not run.
    [[nodiscard]] auto run(int argc, const char *const *argv) noexcept -> int
    {
        try
        {
            if (this->parse_arguments(argc, argv))
            {
                this->selectedTests = this->select_tests();

                if (this->list)
                {
                    this->list_tests();
                }
                else
                {
                    this->begin_run();
                    this->run_test_suites();
                    return this->end_run();
                }
            }

            return 0;
        }
        catch (...)
        {
            //Supress any further exceptions
            return -1;
        }
    }

private:
    auto begin_run() -> void
    {
        this->printer.begin_run(Reporter::stats(this->selectedTests));
    }

    [[nodiscard]] auto end_run() -> int
    {
        const Results results = Reporter::results(this->selectedTests);
        this->printer.end_run(results, this->selectedTests);
        return static_cast<int>(results.failures);
    }

    auto list_tests() -> void
    {
        for (const SelectedTests &suite : this->selectedTests)
        {
            this->printer.list_test_suite(*suite.suite);

            for (const Test *test : suite.tests)
            {
                this->printer.list_test(*test);
            }
        }
    }

    [[nodiscard]] auto parse_arguments(int argc, const char *const *argv) -> bool
    {
        ::acommandline::CommandLine parser{this->printer.output_stream()};
        parser.option().long_name("list").short_name('l').description("Lists test suites and tests. Can be combined with test filters and selections.").bind_to(&this->list);
        parser.option().long_name("test").short_name('t').description("Select tests matching the pattern to run. Allows leading and trailing wildcard: *. E.g. *test, *test*, test*.").bind_to(&this->filters.tests);
        parser.option().long_name("suite").short_name('s').description("Select test suites matching the pattern to run. Allows leading and trailing wildcard: *. E.g. *suite, *suite*, suite*.").bind_to(&this->filters.suites);
        parser.option().long_name("filter-test").description("Skips tests matching the pattern. Allows leading and trailing wildcard: *. E.g. *test, *test*, test*.").bind_to(&this->filters.testFilters);
        parser.option().long_name("filter-suite").description("Skips test suites matching the pattern. Allows leading and trailing wildcard: *. E.g. *suite, *suite*, suite*.").bind_to(&this->filters.suiteFilters);
        return argv == nullptr || parser.parse(argc, argv);
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

    auto run_tests(const std::vector<Test *> &tests) -> void
    {
        for (Test *test : tests)
        {
            this->run_test(*test);
        }
    }

    auto run_test_suite(const SelectedTests &tests) -> void
    {
        this->printer.begin_test_suite(*tests.suite);
        this->run_tests(tests.tests);
        this->printer.end_test_suite(*tests.suite);
    }

    auto run_test_suites() -> void
    {
        for (const SelectedTests &suite : this->selectedTests)
        {
            this->run_test_suite(suite);
        }
    }

    [[nodiscard]] auto select_tests(TestSuite &suite) const -> std::vector<Test *>
    {
        std::vector<Test *> tests;

        for (Test &test : suite.tests)
        {
            if (this->filter.is_test_selected(test.name))
            {
                tests.push_back(&test);
            }
        }

        return tests;
    }

    auto select_tests(TestSuite &suite, std::vector<SelectedTests> &selected) const -> void
    {
        std::vector<Test *> tests = this->select_tests(suite);

        if (!tests.empty())
        {
            selected.emplace_back(SelectedTests{
                .suite = &suite,
                .tests = std::move(tests)});
        }
    }

    [[nodiscard]] auto select_tests() const -> std::vector<SelectedTests>
    {
        ::atest::test_context().reset();
        ::atest::test_context().sort_test_suites();

        std::vector<SelectedTests> selected;

        for (TestSuite &suite : ::atest::test_context().test_suites())
        {
            if (this->filter.is_suite_selected(suite.name))
            {
                this->select_tests(suite, selected);
            }
        }

        return selected;
    }

    std::vector<SelectedTests> selectedTests;
    Filters filters;
    TestFilter filter{this->filters};
    TestContext context;
    Printer printer;
    bool list = false;
};
}
