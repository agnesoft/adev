#ifndef __clang__
module atest : test_filter;
import acommandline;
#endif

namespace atest
{
class TestFilter
{
public:
    TestFilter(int argc, const char *const *argv, std::ostream &stream)
    {
        if (argc > 0 && argv != nullptr)
        {
            ::acommandline::CommandLine parser{stream};
            parser.option().long_name("test").short_name('t').description("").bind_to(&this->tests);
            parser.option().long_name("suite").short_name('s').description("").bind_to(&this->suites);
            parser.option().long_name("filter-test").description("").bind_to(&this->filteredTests);
            parser.option().long_name("filter-suite").description("").bind_to(&this->filteredSuites);
            parser.parse(argc, argv);
        }
    }

    [[nodiscard]] auto is_test_selected(const std::string &name) const -> bool
    {
        if (this->tests.empty())
        {
            for (const std::string &filter : this->filteredTests)
            {
                if (!name.empty())
                {
                    if (filter.front() == '*')
                    {
                        if (name.ends_with(filter.substr(1)))
                        {
                            return false;
                        }
                    }
                    else if (filter.back() == '*' && !filter.ends_with("\\*"))
                    {
                        if (name.starts_with(filter.substr(0, filter.size() - 1)))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (filter == name)
                        {
                            return false;
                        }
                    }
                }
            }

            return true;
        }
        else
        {
            for (const std::string &filter : this->tests)
            {
                if (!filter.empty())
                {
                    if (filter.front() == '*')
                    {
                        if (name.ends_with(filter.substr(1)))
                        {
                            return true;
                        }
                    }
                    else if (filter.back() == '*' && !filter.ends_with("\\*"))
                    {
                        if (name.starts_with(filter.substr(0, filter.size() - 1)))
                        {
                            return true;
                        }
                    }
                    else
                    {
                        if (filter == name)
                        {
                            return true;
                        }
                    }
                }
            }

            return false;
        }
    }

    [[nodiscard]] auto is_suite_selected([[maybe_unused]] const std::string &name) const -> bool
    {
        return true;
    }

private:
    std::vector<std::string> suites;
    std::vector<std::string> tests;
    std::vector<std::string> filteredSuites;
    std::vector<std::string> filteredTests;
};
}
