#ifndef __clang__
module atest : results;
import : stats;
#endif

namespace atest
{
//! \private
struct Results : Stats
{
    std::size_t failedTests = 0;
    std::size_t expectations = 0;
    std::size_t failedExpectations = 0;
    std::size_t failures = 0;
    std::chrono::microseconds duration = std::chrono::microseconds::zero();
};
}
