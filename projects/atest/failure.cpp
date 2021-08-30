#ifndef __clang__
module atest : failure;
import astl;
#endif

namespace atest
{
//! Test failure and its details.
struct Failure
{
    std::string what;
    std::string expected;
    std::string actual;
    std::source_location sourceLocation;
};
}
