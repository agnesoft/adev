#ifndef __clang__
export module awinapi : last_error_message;
export import : buffer;
#endif

#ifdef _WIN32
namespace awinapi
{
//! Uses the last error code reported from
//! ::GetErrorCode() WinAPI function to extract
//! and format its textual explanation.
export [[nodiscard]] auto last_error_message() -> std::string
{
    DWORD error = ::GetLastError();

    if (error == 0U)
    {
        return {};
    }

    Buffer message;

    message.tchar_size() = ::FormatMessage(static_cast<DWORD>(FORMAT_MESSAGE_ALLOCATE_BUFFER)
                                               | static_cast<DWORD>(FORMAT_MESSAGE_FROM_SYSTEM)
                                               | static_cast<DWORD>(FORMAT_MESSAGE_IGNORE_INSERTS),
                                           nullptr,
                                           error,
                                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // NOLINT(hicpp-signed-bitwise)
                                           static_cast<LPTSTR>(static_cast<void *>(&message.str())),
                                           0,
                                           nullptr);
    return std::string(message.str(), message.byte_size()) + " (" + std::to_string(error) + ')';
}
}
#endif
