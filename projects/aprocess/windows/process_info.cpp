#ifndef __clang__
module;
#endif

#if __has_include(<Windows.h>)
#    pragma warning(push)
#    pragma warning(disable : 5105)
#    pragma warning(disable : 5106)
#    pragma warning(disable : 4005)
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#    include <strsafe.h>
#    pragma warning(pop)
#endif

#ifndef __clang__
module aprocess : process_info;
#    pragma warning(push)
#    pragma warning(disable : 5105)
#    pragma warning(disable : 5106)
#    pragma warning(disable : 4005)
import : windows_h;
#    pragma warning(pop)
#endif

#ifdef _WIN32
namespace aprocess
{
class ProcessInfo
{
public:
    ProcessInfo() = default;
    ProcessInfo(const ProcessInfo &other) = delete;
    ProcessInfo(ProcessInfo &&other) noexcept = default;
    ~ProcessInfo() = default;

    [[nodiscard]] constexpr auto get() const noexcept -> const ::PROCESS_INFORMATION &
    {
        return this->processInfo;
    }

    [[nodiscard]] constexpr auto get() noexcept -> ::PROCESS_INFORMATION &
    {
        return this->processInfo;
    }

    auto operator=(const ProcessInfo &other) -> ProcessInfo & = delete;
    auto operator=(ProcessInfo &&other) noexcept -> ProcessInfo & = default;

private:
    ::PROCESS_INFORMATION processInfo{};
};
}
#endif
