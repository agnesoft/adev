#ifndef __clang__
module athreadpool:job;
import astl;
#endif

namespace athreadpool
{
//! \private
struct Job
{
    std::future<void> work;
    std::mutex mutex;
};
}
