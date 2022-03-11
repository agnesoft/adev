#ifndef __clang__
module athreadpool : queue;
import astl;
#endif

namespace athreadpool
{
//! \private
class Queue
{
public:
    [[nodiscard]] auto front() -> std::function<void()> &
    {
        std::lock_guard<std::mutex> lock{this->mutex};
        return this->queue.front();
    }

    [[nodiscard]] auto empty() -> bool
    {
        std::lock_guard<std::mutex> lock{this->mutex};
        return this->queue.empty();
    }

    auto pop() -> void
    {
        std::lock_guard<std::mutex> lock{this->mutex};
        this->queue.pop();
    }

    auto push(std::function<void()> callable) -> void
    {
        std::lock_guard<std::mutex> lock{this->mutex};
        this->queue.push(std::move(callable));
    }

private:
    std::queue<std::function<void()>> queue;
    std::mutex mutex;
};
}
