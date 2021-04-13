#ifdef _MSC_VER
export module acore : process;

import : acore_common;
#endif

namespace acore
{
//! The Process is a simple cross-platform class
//! that provides ability to start processes.
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
        TinyProcessLib::Process process{command(),
                                        workingDirectory(),
                                        [&](const char *output, std::size_t len) {
                                            mOutput = std::string(output, len);
                                        },
                                        [&](const char *error, std::size_t len) {
                                            mError = std::string(error, len);
                                        }};
        return process.get_exit_status();
    }

    std::string mCommand;
    std::string mError;
    std::string mOutput;
    std::string mWorkingDirectory;
    int mExitCode = 0;
};
}