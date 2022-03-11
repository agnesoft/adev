#ifndef __clang__
module athreadpool : monitor;
import : jobs;
import : queue;
#endif

namespace athreadpool
{
//! \private
class Monitor
{
public:
    Monitor(Jobs &jobs, Queue &queue) :
        jobs{jobs},
        queue{queue}
    {
    }

    Monitor(const Monitor &other) = delete;
    Monitor(Monitor &&other) noexcept = delete;

    ~Monitor()
    {
        this->running.store(false);
    }

    auto launch() -> void
    {
        bool previous = false;

        if (!this->running.compare_exchange_strong(previous, true))
        {
            return;
        }

        this->start();
    }

    auto operator=(const Monitor &other) -> Monitor & = delete;
    auto operator=(Monitor &&other) noexcept -> Monitor & = delete;

private:
    auto start() -> void
    {
        this->monitor = std::async([&]() {
            while (this->running.load())
            {
                if (this->jobs.assign(this->queue.front()))
                {
                    this->queue.pop();

                    if (this->queue.empty())
                    {
                        this->running.store(false);
                    }
                }
            }
        });
    }

    Jobs &jobs;
    Queue &queue;
    std::future<void> monitor;
    std::atomic_bool running;
};
}
