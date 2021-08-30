#ifndef __clang__
module atest : failed_assertion;
#endif

namespace atest
{
//! Exception type to indicate an assertion
//! failed.
struct FailedAssertion
{
};
}
