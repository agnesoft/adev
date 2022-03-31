#ifndef __clang__
module athreadpool:queue;
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
        const std::lock_guard lock{this->mutex};
        return this->queue.front();
    }

    [[nodiscard]] auto empty() -> bool
    {
        const std::lock_guard lock{this->mutex};
        return this->queue.empty();
    }

    auto pop() -> void
    {
        const std::lock_guard lock{this->mutex};
        this->queue.pop();
    }

    auto push(std::function<void()> callable) -> void
    {
        const std::lock_guard lock{this->mutex};
        this->queue.push(std::move(callable));
    }

private:
    std::queue<std::function<void()>> queue;
    std::mutex mutex;
};
}
