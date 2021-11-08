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
    explicit WindowsProcess(const ProcessSetup &setup, Process &process)
    {
        this->setup_read_pipe(setup, process);
        this->setup_write_pipe(setup);
        std::string commandLine = WindowsProcess::create_command_line(setup.command, setup.arguments);

        if (::CreateProcess(nullptr,
                            &commandLine.front(),
                            nullptr,
                            nullptr,
                            TRUE,
                            0,
                            nullptr,
                            setup.workingDirectory.c_str(),
                            &this->startupInfo,
                            &this->processInfo)
            == FALSE)
        {
            throw std::runtime_error{"Failed to create process:\n  " + ::awinapi::last_error_message()};
        }
    }

    WindowsProcess(const WindowsProcess &other) = delete;
    WindowsProcess(WindowsProcess &&other) noexcept = default;

    ~WindowsProcess() = default;

    [[nodiscard]] auto exit_code() const -> int
    {
        int exitCode = 0;

        if (::GetExitCodeProcess(this->processInfo.hProcess,
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
        this->do_write("\n");
    }

    WindowsProcess &operator=(const WindowsProcess &other) = delete;
    WindowsProcess &operator=(WindowsProcess &&other) noexcept = default;

private:
    [[nodiscard]] static auto create_command_line(const std::string &command, const std::vector<std::string> &arguments) -> std::string
    {
        std::string args = command;

        for (const std::string &arg : arguments)
        {
            args += ' ';
            args += arg;
        }

        return args;
    }

    auto do_wait(DWORD milliseconds) const -> bool
    {
        switch (::WaitForSingleObject(this->processInfo.hProcess, milliseconds))
        {
        case WAIT_OBJECT_0:
            return true;
        case WAIT_FAILED:
            throw std::runtime_error{"Wait for process failed:\n  " + ::awinapi::last_error_message()};
        case WAIT_ABANDONED:
            [[fallthrough]];
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

    auto setup_write_pipe(const ProcessSetup &setup) -> void
    {
        this->writePipe = std::make_unique<::awinapi::Pipe>();
        this->startupInfo.dwFlags = STARTF_USESTDHANDLES;
        this->startupInfo.hStdInput = this->writePipe->read_handle();
        ::SetHandleInformation(this->writePipe->write_handle(), HANDLE_FLAG_INHERIT, 0);

        if (!setup.write)
        {
            ::CloseHandle(this->writePipe->write_handle());
            this->writePipe->write_handle() = nullptr;
        }
    }

    auto setup_read_pipe(const ProcessSetup &setup, Process &process) -> void
    {
        this->readPipe = std::make_unique<::awinapi::Pipe>();
        this->startupInfo.dwFlags = STARTF_USESTDHANDLES;
        this->startupInfo.hStdError = this->readPipe->write_handle();
        this->startupInfo.hStdOutput = this->readPipe->write_handle();
        ::SetHandleInformation(this->readPipe->read_handle(), HANDLE_FLAG_INHERIT, 0);

        if (setup.read)
        {
            this->asyncReader = std::make_unique<AsyncReader>(this->readPipe->read_handle(), setup, process);
        }
        else
        {
            ::CloseHandle(this->readPipe->read_handle());
            this->readPipe->read_handle() = nullptr;
        }
    }

    ::STARTUPINFO startupInfo{.cb = sizeof(this->startupInfo)};
    ::PROCESS_INFORMATION processInfo{};
    std::unique_ptr<AsyncReader> asyncReader;
    std::unique_ptr<::awinapi::Pipe> readPipe;
    std::unique_ptr<::awinapi::Pipe> writePipe;
};
}
#endif
