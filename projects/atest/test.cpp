#ifndef __clang__
export module atest:test;
import :failure;
#endif

namespace atest
{
//! \private
export struct Test
{
    std::string name;
    auto(*body)() -> void = nullptr;
    std::source_location sourceLocation;
    std::size_t expectations = 0;
    std::size_t failedExpectations = 0;
    std::chrono::microseconds duration = std::chrono::microseconds::zero();
    std::vector<Failure> failures;
};
}
