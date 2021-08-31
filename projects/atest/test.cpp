#ifndef __clang__
module atest : test;
import : failure;
#endif

namespace atest
{
//! Test details and results.
struct Test
{
    std::string name;
    auto (*body)() -> void = nullptr;
    std::source_location sourceLocation;
    size_t expectations = 0;
    size_t failedExpectations = 0;
    std::chrono::microseconds duration = std::chrono::microseconds::zero();
    std::vector<Failure> failures;
};
}
