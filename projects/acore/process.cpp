#ifdef _MSC_VER
module;

#    pragma warning(push)
#    pragma warning(disable : 5105)
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#    pragma warning(pop)

export module acore : process;

import : acore_common;
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
        mExitCode = executeProcess();
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
    [[nodiscard]] auto executeProcess() -> int
    {
#ifdef _MSC_VER
        PROCESS_INFORMATION processInfo;
        SecureZeroMemory(&processInfo, sizeof(processInfo));

        STARTUPINFO startupInfo;
        SecureZeroMemory(&startupInfo, sizeof(processInfo));
        startupInfo.cb = sizeof(startupInfo);

        std::cout << "starting...\n";
        BOOL processCreated = CreateProcess(mCommand.data(), nullptr, nullptr, nullptr, FALSE, 0, nullptr, mWorkingDirectory.data(), &startupInfo, &processInfo);
        DWORD exitCode = {};
        std::cout << "* started *\n";

        if (processCreated == FALSE)
        {
            std::cout << "failed to start\n";
            exitCode = GetLastError();
        }
        else
        {
            std::cout << "Wait for it to finish\n";
            WaitForSingleObject(processInfo.hProcess, INFINITE);
            std::cout << "It finished\n";

            BOOL exitCodeRetrieved = GetExitCodeProcess(processInfo.hProcess, &exitCode);

            if (exitCodeRetrieved == FALSE)
            {
                std::cout << "Failed to get exit code\n";
                exitCode = GetLastError();
            }

            std::cout << "Close handles.\n";
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        }

        return static_cast<int>(exitCode);
#else

#endif
    }

    std::string mCommand;
    std::string mError;
    std::string mOutput;
    std::string mWorkingDirectory;
    int mExitCode = 0;
};
}