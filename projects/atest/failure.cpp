#ifndef __clang__
module atest : failure;
import astl;
#endif

namespace atest
{
//! \private
struct Failure
{
    std::string what;
    std::string expected;
    std::string actual;
    std::source_location sourceLocation;
};
}
