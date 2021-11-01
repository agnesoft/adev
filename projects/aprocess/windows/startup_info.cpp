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
    StartupInfo(WindowsPipe &outPipe, WindowsPipe &inPipe) :
        startupInfo{
            .cb = sizeof(this->startupInfo),
            .dwFlags = STARTF_USESTDHANDLES,
            .hStdInput = inPipe.read_handle(),
            .hStdOutput = outPipe.write_handle(),
            .hStdError = outPipe.write_handle()}
    {
        ::SetHandleInformation(inPipe.write_handle(), HANDLE_FLAG_INHERIT, 0);
        ::SetHandleInformation(outPipe.read_handle(), HANDLE_FLAG_INHERIT, 0);
    }

    [[nodiscard]] constexpr auto get() noexcept -> ::STARTUPINFO &
    {
        return this->startupInfo;
    }

private:
    ::STARTUPINFO startupInfo;
};
}
#endif
