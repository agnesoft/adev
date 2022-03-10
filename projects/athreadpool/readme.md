# atrheadpool

-   [Overview](#overview)
-   [Prerequisites](#prerequisites)
-   [Usage](#usage)

# Overview

Throttled asynchronous job queue.

# Prerequisites

-   C++ compiler
-   C++20
-   Exceptions
-   [astl](/projects/astl/readme.md)

# Usage

At a glance:

```
import athreadpool;

auto main(int argc, char *argv[]) -> void
{
    ::athreadpool::ThreadPool pool; //creates thread pool with std::thread::hardware_concurrency() parallel jobs available
    std::atomic_int result;

    for (int i = 0; i < 100; ++i) //queue 100 jobs to be run asynchronously
    {
        pool.run([&] {
            ++result;
        });
    }

    pool.wait(std::chrono::milliseconds{100}); //wait for the jobs to finish
}
```

The `ThreadPool` can be constructed with arbitrary number of concurrent jobs. While a good rule of thumb is to use the default value of `std::thread::hardware_concurrency()` it might not always be the optimal value. For example if the jobs to be executed cannot make full use of the CPU all the time it might be beneficial to increase the number. Conversely if the goal is to preserve systems's responsiveness it might be prudent to limit the number of concurrent jobs below the number of threads available on the system.

Note that it is not possible to cancel already running jobs and the destructor of the `ThreadPool` will always wait for them to finish possibly resulting in a deadlock if the tasks being executed are not well behaved. However the jobs that were only queued but not yet assigned to a job will be discarded upon destruction.
