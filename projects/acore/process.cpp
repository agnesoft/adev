#ifdef _MSC_VER
export module acore : process;

import : acore_common;
import : process_windows;
#endif

namespace acore
{
//! The Process is a cross-platform class
//! that provides ability to start a processes.
//!
//! The process is run synchronosouly in the constructor.
//! When the construcor finishes you can access the output
//! with convenience functions such as exitCode(), standardOutput()
//! and standardError().
//!
//! \note While the Process class is cross-platform the commands
//! to run are still platform specific in most cases.
export class Process
{
public:
    //! Constructs the Process object and executes \a command.
    explicit Process(std::string command) :
        Process{std::move(command), std::filesystem::current_path().string()}
    {
    }

    //! Constructs the Process object and executes \a command
    //! as if run from the \a workingDirectory.
    Process(std::string command, std::string workingDirectory) :
        mCommand{std::move(command)},
        mWorkingDirectory{std::move(workingDirectory)}
    {
        WindowsProcess process;
        process.start(&mCommand, mWorkingDirectory);
        mExitCode = process.wait();
        mOutput = process.stdout();
        mError = process.stderr();

        //executeProcess();
    }

    //! Returns the command passed in during construction.
    [[nodiscard]] auto command() const noexcept -> const std::string &
    {
        return mCommand;
    }

    //! Returns the return value (exit code) of the run command.
    [[nodiscard]] auto exitCode() const noexcept -> int
    {
        return mExitCode;
    }

    //! Returns the standard error contents of the run command.
    [[nodiscard]] auto standardError() const noexcept -> const std::string &
    {
        return mError;
    }

    //! Returns the standard output contents of the run command.
    [[nodiscard]] auto standardOutput() const noexcept -> const std::string &
    {
        return mOutput;
    }

    //! Returns the currently used working directory.
    [[nodiscard]] auto workingDirectory() const noexcept -> const std::string &
    {
        return mWorkingDirectory;
    }

private:
#ifdef _MSC_VER
    [[nodiscard]] auto executeProcess() -> void
    {
        HANDLE stdoutRead = nullptr;
        HANDLE stdoutWrite = nullptr;
        HANDLE stderrRead = nullptr;
        HANDLE stderrWrite = nullptr;

        SECURITY_ATTRIBUTES securityAttributes;
        ZeroMemory(&securityAttributes, sizeof(securityAttributes));
        securityAttributes.nLength = sizeof(securityAttributes);
        securityAttributes.bInheritHandle = TRUE;

        if (CreatePipe(&stdoutRead, &stdoutWrite, &securityAttributes, 0) != TRUE)
        {
            throw 1;
        }

        if (CreatePipe(&stderrRead, &stderrWrite, &securityAttributes, 0) != TRUE)
        {
            throw 1;
        }

        STARTUPINFO startupInfo;
        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        startupInfo.hStdOutput = stdoutWrite;
        startupInfo.hStdError = stderrWrite;
        startupInfo.dwFlags |= STARTF_USESTDHANDLES;

        PROCESS_INFORMATION processInfo;
        ZeroMemory(&processInfo, sizeof(processInfo));

        if (CreateProcess(nullptr,
                          &mCommand.front(),
                          nullptr,
                          nullptr,
                          TRUE,
                          0,
                          nullptr,
                          mWorkingDirectory.c_str(),
                          &startupInfo,
                          &processInfo)
            == TRUE)
        {
            WaitForSingleObject(processInfo.hProcess, INFINITE);
            CloseHandle(stdoutWrite);
            CloseHandle(stderrWrite);

            if (GetExitCodeProcess(processInfo.hProcess, static_cast<LPDWORD>(static_cast<void *>(&mExitCode))) == FALSE)
            {
                mExitCode = static_cast<int>(GetLastError());
            }

            std::cout << "Read stuff\n";
            const size_t BUFFER_SIZE = 65536;
            char buffer[BUFFER_SIZE] = {};
            DWORD bytesRead = 0;

            while (ReadFile(stdoutRead,
                            static_cast<LPVOID>(buffer),
                            static_cast<DWORD>(BUFFER_SIZE),
                            &bytesRead,
                            nullptr)
                       == TRUE
                   && bytesRead != 0)
            {
                mOutput.append(buffer, bytesRead);
            }

            while (ReadFile(stderrRead,
                            static_cast<LPVOID>(buffer),
                            static_cast<DWORD>(BUFFER_SIZE),
                            &bytesRead,
                            nullptr)
                       == TRUE
                   && bytesRead != 0)
            {
                mError.append(buffer, bytesRead);
            }

            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        }
        else
        {
            mExitCode = static_cast<int>(GetLastError());
        }

        CloseHandle(stdoutRead);
        CloseHandle(stderrRead);
    }
#endif

    std::string mCommand;
    std::string mError;
    std::string mOutput;
    std::string mWorkingDirectory;
    int mExitCode = 0;
};
}
