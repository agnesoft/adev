#ifndef __clang__
module aprocess : windows_process;
import : process_setup;
import awinapi;
#endif

#ifdef _WIN32
namespace aprocess
{
//! \private
class WindowsProcess
{
public:
    explicit WindowsProcess(const ProcessSetup &setup) :
        setup{&setup}
    {
        this->setup_read_pipe();
        this->setup_write_pipe();
        std::string commandLine = this->create_command_line();
        std::string environment = this->create_environment();

        if (::CreateProcess(nullptr,
                            &commandLine.front(),
                            nullptr,
                            nullptr,
                            TRUE,
                            this->process_flags(),
                            environment.empty() ? nullptr : environment.data(),
                            this->setup->workingDirectory.c_str(),
                            &this->startupInfo,
                            &this->processInformation)
            == FALSE)
        {
            throw std::runtime_error{"Failed to create process:\n  " + ::awinapi::last_error_message()};
        }

        this->close_unused_handles();
    }

    WindowsProcess(const WindowsProcess &other) = delete;
    WindowsProcess(WindowsProcess &&other) noexcept = default;

    ~WindowsProcess()
    {
        try
        {
            if (!this->setup->detached && this->is_running())
            {
                this->kill();
            }

            ::CloseHandle(this->processInformation.hProcess);
            ::CloseHandle(this->processInformation.hThread);
        }
        catch (...)
        {
        }
    }

    auto close_unused_handles() -> void
    {
        this->readPipe.close_write();
        this->writePipe.close_read();
    }

    [[nodiscard]] auto exit_code() const -> int
    {
        int exitCode = 0;

        if (::GetExitCodeProcess(this->processInformation.hProcess,
                                 static_cast<LPDWORD>(static_cast<void *>(&exitCode)))
            == FALSE)
        {
            throw std::runtime_error{"Failed to get exit code from ProcessWindows:\n  " + ::awinapi::last_error_message()};
        }

        return exitCode;
    }

    [[nodiscard]] auto is_running() const -> bool
    {
        return !this->do_wait(0);
    }

    auto kill() const -> void
    {
        ::TerminateProcess(this->processInformation.hProcess, 1U);
    }

    [[nodiscard]] auto pid() const noexcept -> std::int64_t
    {
        return static_cast<std::int64_t>(this->processInformation.dwProcessId);
    }

    [[nodiscard]] auto read(std::chrono::milliseconds timeout) -> std::string
    {
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now() + timeout;

        do
        {
            DWORD bytesAvailable = 0;
            ::PeekNamedPipe(this->readPipe.read_handle(),
                            nullptr,
                            0,
                            nullptr,
                            &bytesAvailable,
                            nullptr);

            if (bytesAvailable != 0)
            {
                std::string buffer = std::string(bytesAvailable, char{});
                DWORD bytesRead = 0;
                ::ReadFile(this->readPipe.read_handle(),
                           static_cast<LPVOID>(buffer.data()),
                           bytesAvailable,
                           &bytesRead,
                           nullptr);
                return buffer;
            }
        }
        while (this->is_running() && std::chrono::system_clock::now() < end);

        return {};
    }

    auto terminate() const -> void
    {
        ::PostThreadMessage(this->processInformation.dwThreadId, WM_CLOSE, 0, 0);
        ::GenerateConsoleCtrlEvent(CTRL_C_EVENT, this->processInformation.dwProcessId);
    }

    auto wait(std::chrono::milliseconds timeout) -> void
    {
        if (!this->do_wait(static_cast<DWORD>(timeout.count())))
        {
            throw std::runtime_error{"Wait for process timed out."};
        }
    }

    auto write(const std::string &message) -> void
    {
        if (!this->is_running())
        {
            throw std::runtime_error{"Failed to write to process:\n  The process is not running."};
        }

        DWORD bytesWritten = 0;

        if (::WriteFile(this->writePipe.write_handle(),
                        message.c_str(),
                        static_cast<DWORD>(message.size()),
                        &bytesWritten,
                        nullptr)
                == FALSE
            || bytesWritten != message.size())
        {
            throw std::runtime_error{"Failed to write to process: (" + std::to_string(bytesWritten) + " of " + std::to_string(message.size()) + " written)\n" + ::awinapi::last_error_message()};
        }
    }

    auto operator=(const WindowsProcess &other) -> WindowsProcess & = delete;
    auto operator=(WindowsProcess &&other) noexcept -> WindowsProcess & = default;

private:
    [[nodiscard]] auto create_command_line() -> std::string
    {
        std::string args = this->setup->command;

        for (const std::string &arg : this->setup->arguments)
        {
            args += ' ';
            args += arg;
        }

        return args;
    }

    [[nodiscard]] auto create_environment() -> std::string
    {
        if (this->setup->environment.empty())
        {
            return {};
        }

        return this->new_environment_variables()
            + WindowsProcess::existing_environment()
            + '\0';
    }

    [[nodiscard]] auto do_wait(DWORD milliseconds) const -> bool
    {
        switch (::WaitForSingleObject(this->processInformation.hProcess, milliseconds))
        {
        case WAIT_OBJECT_0:
            return true;
        case WAIT_ABANDONED:
            [[fallthrough]];
        case WAIT_FAILED:
            throw std::runtime_error{"Wait for process failed:\n  " + ::awinapi::last_error_message()};
        case WAIT_TIMEOUT:
            break;
        }

        return false;
    }

    [[nodiscard]] static auto existing_environment() -> std::string
    {
        std::string envString;

        LPTCH env = ::GetEnvironmentStrings();
        auto *var = static_cast<LPTSTR>(env);

        while (*var != '\0')
        {
            auto size = static_cast<std::size_t>(::lstrlen(var));
            envString.append(var, size + 1);
            var += size + 1; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }

        ::FreeEnvironmentStrings(env);

        return envString;
    }

    [[nodiscard]] auto new_environment_variables() -> std::string
    {
        std::string variables;

        for (const EnvironmentVariable &envVar : this->setup->environment)
        {
            variables.append(envVar.name + '=' + envVar.value + '\0');
        }

        return variables;
    }

    [[nodiscard]] auto process_flags() const noexcept -> DWORD
    {
        if (this->setup->detached)
        {
            return CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS;
        }
        else
        {
            return 0;
        }
    }

    auto setup_write_pipe() -> void
    {
        this->startupInfo.dwFlags = STARTF_USESTDHANDLES;
        this->startupInfo.hStdInput = this->writePipe.read_handle();
        ::SetHandleInformation(this->writePipe.write_handle(), HANDLE_FLAG_INHERIT, 0);
    }

    auto setup_read_pipe() -> void
    {
        this->startupInfo.dwFlags = STARTF_USESTDHANDLES;
        this->startupInfo.hStdError = this->readPipe.write_handle();
        this->startupInfo.hStdOutput = this->readPipe.write_handle();
        ::SetHandleInformation(this->readPipe.read_handle(), HANDLE_FLAG_INHERIT, 0);
    }

    const ProcessSetup *setup = nullptr;
    ::STARTUPINFO startupInfo{.cb = sizeof(this->startupInfo)};
    ::PROCESS_INFORMATION processInformation{};
    ::awinapi::Pipe readPipe;
    ::awinapi::Pipe writePipe;
};
}
#endif
