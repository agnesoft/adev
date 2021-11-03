export module awinapi;

export import "windows.hpp";
export import astl;

#ifdef _WIN32
namespace awinapi
{
export [[nodiscard]] auto last_error_message() -> std::string
{
    LPVOID message = nullptr;
    auto error = ::GetLastError();

    ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr,
                    error,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    static_cast<LPTSTR>(static_cast<void *>(&message)),
                    0,
                    nullptr);

    const auto size = ::lstrlen(static_cast<LPCTSTR>(message)) * sizeof(TCHAR);
    LPVOID buffer = ::LocalAlloc(LMEM_ZEROINIT, size);

    ::StringCchPrintf(static_cast<LPTSTR>(buffer),
                      ::LocalSize(buffer) / sizeof(TCHAR),
                      TEXT("%s (%d)"),
                      message,
                      error);

    const std::string output(static_cast<LPCTSTR>(buffer), size);

    ::LocalFree(message);
    ::LocalFree(buffer);

    return output;
}
}
#endif
