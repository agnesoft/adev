#ifndef __clang__
export module athreadpool : threadpool;
import : monitor;
#endif

namespace athreadpool
{
//! The `ThreadPool` class implements throttled
//! asynchronous job queue.
//!
//! It is constructed with the number of jobs that
//! can be run concurrently or
//! `std::thread::hardware_concurrency()` by
//! default. The `run()` method takes any `void()`
//! callable (e.g. `[&] { /* ... */}`,
//! `std::function<void()>`, `std::bind` etc.). If
//! there is a free job the callable will be
//! immediately assigned to it. Otherwise it will
//! be queued and a job monitor will schedule it
//! once a job slot becomes available.
//!
//! You can wait() for the jobs to be completed.
//! Upon destruction any already assigned jobs
//! will be awaited and all queued unassigned jobs
//! will be discarded.
export class ThreadPool
{
public:
    //! Constructs the `ThreadPool` with default
    //! number of concurrent jobs equal to
    //! `std::thread::hardware_concurrency()`.
    ThreadPool() :
        ThreadPool{std::thread::hardware_concurrency()}
    {
    }

    //! Constructs the `ThreadPool` with `count`
    //! number of concurrent jobs.
    explicit ThreadPool(std::size_t count) :
        jobs{count},
        queue{},
        monitor{this->jobs, this->queue}
    {
    }

    //! Runs `callable` asynchronously using any
    //! of the available free jobs. If there are
    //! no available free jobs adds `callable` to
    //! the queue to be scheduled as soon as a
    //! `job` becomes free (i.e. completes).
    auto run(auto callable) -> void
    {
        if (this->jobs.assign(callable))
        {
            return;
        }

        this->queue.push(callable);
        this->monitor.launch();
    }

    //! Waits up to `timeout` for all the current
    //! & queued jobs to complete. Throws an
    //! exception after `timeout` if there are
    //! still incomplete jobs.
    auto wait(std::chrono::milliseconds timeout) -> void
    {
        const auto end = std::chrono::system_clock::now() + timeout;
        this->wait_for_queue(end);
        this->wait_for_jobs(end);
    }

private:
    auto wait_for_jobs(const auto end) -> void
    {
        while (std::chrono::system_clock::now() < end)
        {
            if (this->jobs.done())
            {
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }

        ThreadPool::timeout();
    }

    auto wait_for_queue(const auto end) -> void
    {
        while (std::chrono::system_clock::now() < end)
        {
            if (this->queue.empty())
            {
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }

        ThreadPool::timeout();
    }

    static auto timeout() -> void
    {
        throw std::runtime_error{"ThreadPool::wait() timed out"};
    }

    Jobs jobs;
    Queue queue;
    Monitor monitor;
};
}
