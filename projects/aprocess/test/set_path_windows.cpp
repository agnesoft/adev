module;

#ifdef _WIN32
#    pragma warning(push)
#    pragma warning(disable : 5105)
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#    include <strsafe.h>
#    pragma warning(pop)
#endif

export module aprocesstest : set_path_windows;

#ifdef _WIN32
#    pragma warning(push)
#    pragma warning(disable : 5105)
#    pragma warning(disable : 5106)
#    pragma warning(disable : 4005)
import astl;
#    pragma warning(pop)

[[nodiscard]] auto last_error_message() -> std::string
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

export auto set_path(const char *app) -> void
{
    const std::string bin = std::filesystem::absolute(std::filesystem::path{app}.parent_path()).string();
    constexpr std::size_t BUFFER_SIZE = 4096;
    char path[BUFFER_SIZE]{};

    if (::GetEnvironmentVariable("PATH", path, BUFFER_SIZE) == FALSE)
    {
        std::cout << "Failed to retrieve environment variable PATH:\n  " << ::last_error_message() << "\n";
    }

    if (::SetEnvironmentVariable("PATH", (bin + ';' + path).c_str()) == FALSE)
    {
        std::cout << "Failed to set environment variable PATH:\n  " << ::last_error_message() << "\n";
    }
}
#endif
