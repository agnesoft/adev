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
module aprocess : security_attributes;
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
class SecurityAttributes
{
public:
    [[nodiscard]] constexpr auto get() noexcept -> ::SECURITY_ATTRIBUTES &
    {
        return this->securityAttributes;
    }

private:
    ::SECURITY_ATTRIBUTES securityAttributes{.nLength = sizeof(this->securityAttributes),
                                             .lpSecurityDescriptor = nullptr,
                                             .bInheritHandle = TRUE};
};
}
#endif
