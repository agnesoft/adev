import aprocess;

#ifdef _WIN32
import awinapi;

[[nodiscard]] auto is_process_running(std::int64_t pid) -> bool
{
    return ::awinapi::Handle{::OpenProcess(
                                 static_cast<DWORD>(PROCESS_QUERY_INFORMATION) | static_cast<DWORD>(PROCESS_VM_READ),
                                 FALSE,
                                 static_cast<DWORD>(pid))}
               .get()
        != nullptr;
}
#else
// Required to avoid importing wait_h (sys/wait.h)
// that causes ICE for clang-tidy as it is also
// imported (but not exported) by aprocess.
extern auto waitpid(__pid_t __pid, int *__stat_loc, int __options) -> __pid_t; //NOLINT(readability-redundant-declaration, bugprone-reserved-identifier)

[[nodiscard]] auto is_process_running(std::int64_t pid) -> bool
{
    int status = 0;
    return ::waitpid(static_cast<int>(pid), &status, WNOHANG) == 0;
}
#endif
