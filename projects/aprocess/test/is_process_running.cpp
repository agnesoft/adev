#ifdef _WIN32
import awinapi;

[[nodiscard]] auto is_process_running(std::int64_t pid) -> bool
{
    return ::awinapi::Handle{OpenProcess(
                                 static_cast<DWORD>(PROCESS_QUERY_INFORMATION) | static_cast<DWORD>(PROCESS_VM_READ),
                                 FALSE,
                                 static_cast<DWORD>(pid))}
               .get()
        != nullptr;
}
#else
import astl;
#    ifdef __clang__
import wait_h;
#    else
import<wait.h>;
#    endif

[[nodiscard]] auto is_process_running(std::int64_t pid) -> bool
{
    int status = 0;
    return ::waitpid(static_cast<int>(pid), &status, WNOHANG) == 0;
}
#endif
