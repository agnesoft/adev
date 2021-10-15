#ifndef __clang__
module atest : test_filter;
import acommandline;
#endif

namespace atest
{
//! \private
class TestFilter
{
public:
    TestFilter(int argc, const char *const *argv, std::ostream &stream)
    {
        if (argc > 0 && argv != nullptr)
        {
            ::acommandline::CommandLine parser{stream};
            parser.option().long_name("test").short_name('t').description("Select tests matching the pattern to run. Allows leading and trailing wildcard: *. E.g. *test, *test*, test*.").bind_to(&this->tests);
            parser.option().long_name("suite").short_name('s').description("Select test suites matching the pattern to run. Allows leading and trailing wildcard: *. E.g. *suite, *suite*, suite*.").bind_to(&this->suites);
            parser.option().long_name("filter-test").description("Skips tests matching the pattern. Allows leading and trailing wildcard: *. E.g. *test, *test*, test*.").bind_to(&this->testFilters);
            parser.option().long_name("filter-suite").description("Skips test suites matching the pattern. Allows leading and trailing wildcard: *. E.g. *suite, *suite*, suite*.").bind_to(&this->suiteFilters);
            parser.parse(argc, argv);
        }
    }

    [[nodiscard]] auto is_test_selected(const std::string &name) const -> bool
    {
        if (this->tests.empty())
        {
            return !TestFilter::match(name, this->testFilters);
        }

        return TestFilter::match(name, this->tests);
    }

    [[nodiscard]] auto is_suite_selected(const std::string &name) const -> bool
    {
        if (this->suites.empty())
        {
            return !TestFilter::match(name, this->suiteFilters);
        }

        return TestFilter::match(name, this->suites);
    }

private:
    [[nodiscard]] static auto has_back_wildcard(const std::string &filter) -> bool
    {
        return filter.ends_with('*');
    }

    [[nodiscard]] static auto has_front_wildcard(const std::string &filter) -> bool
    {
        return filter.starts_with('*');
    }

    [[nodiscard]] static auto match(const std::string &name, const std::vector<std::string> &filters) -> bool
    {
        return std::any_of(filters.begin(), filters.end(), [&](const std::string &filter) { return TestFilter::match_name(name, filter); });
    }

    [[nodiscard]] static auto match_back_wildcard(const std::string &name, const std::string &filter) -> bool
    {
        return name.starts_with(filter.substr(0, filter.size() - 1));
    }

    [[nodiscard]] static auto match_name(const std::string &name, const std::string &filter) -> bool
    {
        if (TestFilter::has_front_wildcard(filter))
        {
            return TestFilter::match_front_wildcard(name, filter);
        }

        if (TestFilter::has_back_wildcard(filter))
        {
            return TestFilter::match_back_wildcard(name, filter);
        }

        return filter == name;
    }

    [[nodiscard]] static auto match_front_wildcard(const std::string &name, const std::string &filter) -> bool
    {
        if (TestFilter::has_back_wildcard(filter))
        {
            return name.find(filter.substr(1, filter.size() - 2)) != std::string::npos;
        }

        return name.ends_with(filter.substr(1));
    }

    std::vector<std::string> suites;
    std::vector<std::string> tests;
    std::vector<std::string> suiteFilters;
    std::vector<std::string> testFilters;
};
}
