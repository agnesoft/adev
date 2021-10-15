#ifndef __clang__
module atest : filters;
import : failure;
#endif

namespace atest
{
//! \private
struct Filters
{
    std::vector<std::string> suites;
    std::vector<std::string> tests;
    std::vector<std::string> suiteFilters;
    std::vector<std::string> testFilters;
};
}
