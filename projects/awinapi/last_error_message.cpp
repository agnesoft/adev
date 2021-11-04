#ifndef __clang__
export module awinapi : last_error_message;
export import : buffer;
export import astl;
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

    if (error == 0u)
    {
        return {};
    }

    Buffer message;

    message.tchar_size() = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                           nullptr,
                                           error,
                                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                           static_cast<LPTSTR>(static_cast<void *>(&message.str())),
                                           0,
                                           nullptr);
    return std::string(message.str(), message.byte_size()) + " (" + std::to_string(error) + ')';
}
}
#endif