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

    auto launch() -> void
    {
        bool previous = false;

        if (!this->running.compare_exchange_strong(previous, true))
        {
            return;
        }

        this->start();
    }

private:
    auto start() -> void
    {
        this->monitor = std::jthread{[&](const std::stop_token &token) {
            while (!token.stop_requested())
            {
                if (this->jobs.assign(this->queue.front()))
                {
                    this->queue.pop();

                    if (this->queue.empty())
                    {
                        running.store(false);
                        return;
                    }
                }
            }
        }};
    }

    Jobs &jobs;
    Queue &queue;
    std::jthread monitor;
    std::atomic_bool running;
};
}
