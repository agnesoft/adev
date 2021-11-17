#ifdef _WIN32
import awinapi;

[[nodiscard]] auto is_process_running(std::int64_t pid) -> bool
{
    ::HANDLE process = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(pid));

    if (process != nullptr)
    {
        ::CloseHandle(process);
        return true;
    }

    return false;
}
#endif
