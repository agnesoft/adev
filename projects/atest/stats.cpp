#ifndef __clang__
module atest : stats;
import astl;
#endif

namespace atest
{
//! \private
struct Stats
{
    std::size_t testSuites = 0;
    std::size_t tests = 0;
};
}
