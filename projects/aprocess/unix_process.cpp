#ifndef __clang__
module aprocess : unix_process;
import : process_setup;
#endif

#ifndef _WIN32
namespace aprocess
{
//! \private
class UnixProcess
{
public:
    explicit UnixProcess(const ProcessSetup &setup) :
        setup{&setup}
    {
    }

    [[nodiscard]] auto exit_code() const -> int
    {
        static_cast<void>(this->setup);
        return 0;
    }

    [[nodiscard]] auto is_running() const -> bool
    {
        static_cast<void>(this->setup);
        return false;
    }

    auto kill() const -> void
    {
        static_cast<void>(this->setup);
    }

    [[nodiscard]] auto pid() const noexcept -> std::int64_t
    {
        static_cast<void>(this->setup);
        return 0;
    }

    auto terminate() const -> void
    {
        static_cast<void>(this->setup);
    }

    auto wait([[maybe_unused]] std::chrono::milliseconds timeout) -> void
    {
        static_cast<void>(this->setup);
    }

    auto write([[maybe_unused]] const std::string &message) -> void
    {
        static_cast<void>(this->setup);
    }

private:
    const ProcessSetup *setup = nullptr;
};
}
#endif
