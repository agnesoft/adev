#ifndef __clang__
module atest : selected_tests;
import : test_suite;
#endif

namespace atest
{
//! \private
struct SelectedTests
{
    TestSuite *suite;
    std::vector<Test *> tests;
};
}
