#ifndef __clang__
module aprocess : windows_process;
import : async_reader;
#endif

#ifdef _WIN32
namespace aprocess
{
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
                            0,
                            environment.empty() ? nullptr : environment.data(),
                            this->setup->workingDirectory.c_str(),
                            &this->startupInfo,
                            &this->processInformation)
            == FALSE)
        {
            throw std::runtime_error{"Failed to create process:\n  " + ::awinapi::last_error_message()};
        }
    }

    WindowsProcess(const WindowsProcess &other) = delete;
    WindowsProcess(WindowsProcess &&other) noexcept = default;

    ~WindowsProcess()
    {
        if (!this->setup->detached)
        {
            this->kill();
        }

        ::CloseHandle(this->processInformation.hProcess);
        ::CloseHandle(this->processInformation.hThread);
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

    auto kill() -> void
    {
        ::TerminateProcess(this->processInformation.hProcess, 1u);
    }

    [[nodiscard]] auto pid() const noexcept -> std::int64_t
    {
        return static_cast<std::int64_t>(this->processInformation.dwProcessId);
    }

    auto terminate() -> void
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
        if (this->writePipe->write_handle() == nullptr)
        {
            throw std::logic_error{"The process does not have stdin pipe open."};
        }

        this->do_write(message);
    }

    WindowsProcess &operator=(const WindowsProcess &other) = delete;
    WindowsProcess &operator=(WindowsProcess &&other) noexcept = default;

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

        return WindowsProcess::existing_environment()
            + this->new_environment_variables()
            + '\0';
    }

    auto do_wait(DWORD milliseconds) const -> bool
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

    auto do_write(const std::string &message) -> void
    {
        DWORD bytesWritten = 0;

        if (::WriteFile(this->writePipe->write_handle(),
                        message.c_str(),
                        static_cast<DWORD>(message.size()),
                        &bytesWritten,
                        nullptr)
            == FALSE)
        {
            throw std::runtime_error{"Failed to write to process:\n  " + ::awinapi::last_error_message()};
        }
    }

    [[nodiscard]] static auto existing_environment() -> std::string
    {
        std::string envString;

        LPTCH env = ::GetEnvironmentStrings();
        auto var = static_cast<LPTSTR>(env);

        while (*var != '\0')
        {
            auto size = static_cast<std::size_t>(::lstrlen(var));
            envString.append(var, size + 1);
            var += size + 1;
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

    auto setup_write_pipe() -> void
    {
        this->writePipe = std::make_unique<::awinapi::Pipe>();
        this->startupInfo.dwFlags = STARTF_USESTDHANDLES;
        this->startupInfo.hStdInput = this->writePipe->read_handle();
        ::SetHandleInformation(this->writePipe->write_handle(), HANDLE_FLAG_INHERIT, 0);

        if (!this->setup->write)
        {
            ::CloseHandle(this->writePipe->write_handle());
            this->writePipe->write_handle() = nullptr;
        }
    }

    auto setup_read_pipe() -> void
    {
        this->readPipe = std::make_unique<::awinapi::Pipe>();
        this->startupInfo.dwFlags = STARTF_USESTDHANDLES;
        this->startupInfo.hStdError = this->readPipe->write_handle();
        this->startupInfo.hStdOutput = this->readPipe->write_handle();
        ::SetHandleInformation(this->readPipe->read_handle(), HANDLE_FLAG_INHERIT, 0);

        if (this->setup->read)
        {
            this->asyncReader = std::make_unique<AsyncReader>(this->readPipe->read_handle(), *this->setup);
        }
        else
        {
            ::CloseHandle(this->readPipe->read_handle());
            this->readPipe->read_handle() = nullptr;
        }
    }

    const ProcessSetup *setup = nullptr;
    ::STARTUPINFO startupInfo{.cb = sizeof(this->startupInfo)};
    ::PROCESS_INFORMATION processInformation{};
    std::unique_ptr<AsyncReader> asyncReader;
    std::unique_ptr<::awinapi::Pipe> readPipe;
    std::unique_ptr<::awinapi::Pipe> writePipe;
};
}
#endif
