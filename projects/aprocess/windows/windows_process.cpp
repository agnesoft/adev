#ifndef __clang__
module;
#endif

#if __has_include(<Windows.h>)
#    pragma warning(push)
#    pragma warning(disable : 5105)
#    pragma warning(disable : 5106)
#    pragma warning(disable : 4005)
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#    include <strsafe.h>
#    pragma warning(pop)
#endif

#ifndef __clang__
module aprocess : windows_process;
#    pragma warning(push)
#    pragma warning(disable : 5105)
#    pragma warning(disable : 5106)
#    pragma warning(disable : 4005)
import : process_setup;
import : startup_info;
import : process_info;
#    pragma warning(pop)
#endif

#ifdef _WIN32
namespace aprocess
{
class WindowsProcess
{
public:
    explicit WindowsProcess(const ProcessSetup &setup)
    {
        StartupInfo startupInfo{this->outPipe, this->inPipe};
        std::string commandLine = WindowsProcess::create_command_line(setup.command, setup.arguments);
        std::cout << commandLine << std::endl;
        if (::CreateProcess(nullptr,
                            &commandLine.front(),
                            nullptr,
                            nullptr,
                            TRUE,
                            0,
                            nullptr,
                            setup.workingDirectory.c_str(),
                            &startupInfo.get(),
                            &this->processInfo.get())
            == FALSE)
        {
            throw std::runtime_error{"Failed to create process:\n  " + last_error_message()};
        }
    }

    WindowsProcess(const WindowsProcess &other) = delete;
    WindowsProcess(WindowsProcess &&other) noexcept = default;

    ~WindowsProcess()
    {
    }

    [[nodiscard]] auto exit_code() const -> int
    {
        int exitCode = 0;

        if (::GetExitCodeProcess(this->processInfo.get().hProcess,
                                 static_cast<LPDWORD>(static_cast<void *>(&exitCode)))
            == FALSE)
        {
            throw std::runtime_error{"Failed to get exit code from ProcessWindows:\n  " + last_error_message()};
        }

        return exitCode;
    }

    [[nodiscard]] auto is_running() const -> bool
    {
        return !this->do_wait(0);
    }

    [[nodiscard]] auto read() -> std::string
    {
        constexpr std::size_t BUFFER_SIZE = 65536;
        static char buffer[BUFFER_SIZE] = {};
        DWORD bytesRead = 0;

        if (::ReadFile(this->outPipe.read_handle(),
                       static_cast<LPVOID>(buffer),
                       static_cast<DWORD>(BUFFER_SIZE),
                       &bytesRead,
                       nullptr)
            == FALSE)
        {
            throw std::runtime_error{"Failed to read from process:\n  " + last_error_message()};
        }

        return std::string(buffer, bytesRead);
    }

    auto wait() -> void
    {
        this->do_wait(INFINITE);
    }

    auto wait(std::chrono::milliseconds timeout) -> bool
    {
        return this->do_wait(static_cast<DWORD>(timeout.count()));
    }

    auto write(const std::string &message) -> void
    {
        DWORD bytesWritten = 0;

        if (::WriteFile(this->inPipe.write_handle(),
                        message.c_str(),
                        static_cast<DWORD>(message.size()),
                        &bytesWritten,
                        nullptr)
            == FALSE)
        {
            throw std::runtime_error{"Failed to write to process:\n  " + last_error_message()};
        }
    }

    WindowsProcess &operator=(const WindowsProcess &other) = delete;
    WindowsProcess &operator=(WindowsProcess &&other) noexcept = default;

private:
    [[nodiscard]] static auto create_command_line(const std::string &executable, const std::vector<std::string> &arguments) -> std::string
    {
        std::string command{'"' + executable + '"'};

        for (const auto &arg : arguments)
        {
            command += ' ';
            command += arg;
        }

        return command;
    }

    auto do_wait(DWORD milliseconds) const -> bool
    {
        switch (::WaitForSingleObject(this->processInfo.get().hProcess, milliseconds))
        {
        case WAIT_OBJECT_0:
            return true;
        case WAIT_FAILED:
            throw std::runtime_error{"Wait failed:\n  " + last_error_message()};
        case WAIT_ABANDONED:
            [[fallthrough]];
        case WAIT_TIMEOUT:
            break;
        }

        return false;
    }

    ProcessInfo processInfo;
    WindowsPipe inPipe;
    WindowsPipe outPipe;
};
}
#endif
