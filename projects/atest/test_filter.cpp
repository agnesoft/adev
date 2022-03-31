#ifndef __clang__
module atest:test_filter;
import :filters;
#endif

namespace atest
{
//! \private
class TestFilter
{
public:
    explicit TestFilter(Filters &filters) :
        filters{filters}
    {
    }

    [[nodiscard]] auto is_test_selected(const std::string &name) const -> bool
    {
        if (this->filters.tests.empty())
        {
            return !TestFilter::match(name, this->filters.testFilters);
        }

        return TestFilter::match(name, this->filters.tests);
    }

    [[nodiscard]] auto is_suite_selected(const std::string &name) const -> bool
    {
        if (this->filters.suites.empty())
        {
            return !TestFilter::match(name, this->filters.suiteFilters);
        }

        return TestFilter::match(name, this->filters.suites);
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

    Filters &filters;
};
}
