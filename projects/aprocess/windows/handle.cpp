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
module aprocess : handle;
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
class Handle
{
public:
    Handle() = default;
    Handle(const Handle &other) = delete;
    Handle(Handle &&other) noexcept = default;

    ~Handle()
    {
        ::CloseHandle(this->handle);
    }

    [[nodiscard]] constexpr auto get() noexcept -> ::HANDLE &
    {
        return this->handle;
    }

    auto operator=(const Handle &other) -> Handle & = delete;
    auto operator=(Handle &&other) noexcept -> Handle & = default;

private:
    ::HANDLE handle = nullptr;
};
}
#endif
