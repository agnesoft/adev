#ifndef __clang__
export module awinapi : pipe;
export import : handle;
export import : last_error_message;
#endif

#ifdef _WIN32
namespace awinapi
{
//! The Pipe is a wrapper around Windows anonymous
//! pipes.
export class Pipe
{
public:
    //! Creates a new anonymous pipe with
    //! `CreatePipe`.
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

    //! Closes the read handle of the pipe.
    auto close_read() -> void
    {
        this->readHandle.close();
    }

    //! Closes the write handle of the pipe.
    auto close_write() -> void
    {
        this->writeHandle.close();
    }

    //! Returns the underlying read handle of the
    //! pipe.
    [[nodiscard]] constexpr auto read_handle() noexcept -> HANDLE
    {
        return this->readHandle.get();
    }

    //! Returns the underlying write handle of the
    //! pipe.
    [[nodiscard]] constexpr auto write_handle() noexcept -> HANDLE
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
