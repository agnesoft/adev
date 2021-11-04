#ifndef __clang__
export module awinapi : pipe;
export import : handle;
export import : last_error_message;
#endif

#ifdef _WIN32
namespace awinapi
{
export class Pipe
{
public:
    Pipe()
    {
        if (::CreatePipe(&this->readHandle.get(),
                         &this->writeHandle.get(),
                         &this->securityAttributes,
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
    ::SECURITY_ATTRIBUTES securityAttributes{.nLength = sizeof(::SECURITY_ATTRIBUTES),
                                             .lpSecurityDescriptor = nullptr,
                                             .bInheritHandle = TRUE};
    Handle readHandle;
    Handle writeHandle;
};
}
#endif
