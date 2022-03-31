#ifndef __clang__
module athreadpool:jobs;
import :job;
#endif

namespace athreadpool
{
//! \private
class Jobs
{
public:
    explicit Jobs(std::size_t count) :
        count{count}
    {
    }

    [[nodiscard]] auto assign(auto callable) -> bool
    {
        for (Job &job : this->jobs)
        {
            if (Jobs::assign(job, callable))
            {
                return true;
            }
        }

        return false;
    }

    [[nodiscard]] auto done() -> bool
    {
        for (Job &job : this->jobs)
        {
            const std::lock_guard lock{job.mutex};

            if (Jobs::is_running(job))
            {
                return false;
            }
        }

        return true;
    }

private:
    [[nodiscard]] static auto assign(Job &job, auto callable) -> bool
    {
        const std::lock_guard lock{job.mutex};

        if (Jobs::is_running(job))
        {
            return false;
        }

        job.work = std::async(callable);
        return true;
    }

    [[nodiscard]] static auto is_running(Job &job) -> bool
    {
        return job.work.valid() && job.work.wait_for(std::chrono::seconds{0}) == std::future_status::timeout;
    }

    std::size_t count = 1;
    std::vector<Job> jobs = std::vector<Job>(this->count);
};
}
