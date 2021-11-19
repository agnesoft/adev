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
import<wait.h>;

[[nodiscard]] auto is_process_running(std::int64_t pid) -> bool
{
    int status = 0;
    return ::waitpid(pid, &status, WNOHANG) == 0;
}
#endif
