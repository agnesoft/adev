#ifndef __clang__
export module atest:test_suite;
import :test;
#endif

namespace atest
{
//! \private
export struct TestSuite // NOLINT(bugprone-exception-escape)
{
    std::string name;
    std::source_location sourceLocation;
    std::vector<Test> tests;
};
}
