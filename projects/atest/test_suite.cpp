#ifndef __clang__
module atest : test_suite;
import : test;
#endif

namespace atest
{
//! Test suite and its tests.
struct TestSuite
{
    std::string name;
    std::source_location sourceLocation;
    std::vector<Test> tests;
};
}
