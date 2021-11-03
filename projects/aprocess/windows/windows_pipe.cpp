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
module aprocess : windows_pipe;
#    pragma warning(push)
#    pragma warning(disable : 5105)
#    pragma warning(disable : 5106)
#    pragma warning(disable : 4005)
import : handle;
import : security_attributes;
#    pragma warning(pop)
#endif

#ifdef _WIN32
namespace aprocess
{
class WindowsPipe
{
public:
    WindowsPipe()
    {
        if (::CreatePipe(&this->readHandle.get(),
                         &this->writeHandle.get(),
                         &this->securityAttributes.get(),
                         0)
            == FALSE)
        {
            throw std::runtime_error{"Failed to create pipe:\n  " + last_error_message()};
        }
    }

    [[nodiscard]] constexpr auto read_handle() noexcept -> HANDLE &
    {
        return this->readHandle.get();
    }

    [[nodiscard]] constexpr auto write_handle() noexcept -> HANDLE &
    {
        return this->writeHandle.get();
    }

private:
    SecurityAttributes securityAttributes;
    Handle readHandle;
    Handle writeHandle;
};
}
#endif
