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
#    pragma warning(pop)
#endif

#ifndef __clang__
module aprocess : startup_info;
#    pragma warning(push)
#    pragma warning(disable : 5105)
#    pragma warning(disable : 5106)
#    pragma warning(disable : 4005)
import : windows_pipe;
#    pragma warning(pop)
#endif

#ifdef _WIN32
namespace aprocess
{
class StartupInfo
{
public:
    [[nodiscard]] constexpr auto get() noexcept -> ::STARTUPINFO &
    {
        return this->startupInfo;
    }

private:
    ::STARTUPINFO startupInfo{
        .cb = sizeof(this->startupInfo)};
};
}
#endif
