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
//! with exitCode() and output() that combines stdout and stderr.
//!
//! \note While the Process class is cross-platform the commands
//! to run are still platform specific in most cases.
export class Process
{
public:
    //! Constructs the Process object and executes \a command.
    explicit Process(std::string command, std::vector<std::string> arguments) :
        Process{std::move(command), std::move(arguments), std::filesystem::current_path().string()}
    {
    }

    //! Constructs the Process object and executes \a command
    //! as if run from the \a workingDirectory.
    Process(std::string command, std::vector<std::string> arguments, std::string workingDirectory) :
        mCommand{std::move(command)},
        mArguments{std::move(arguments)},
        mWorkingDirectory{std::move(workingDirectory)}
    {
#ifdef _MSC_VER
        WindowsProcess process{mCommand, mArguments, mWorkingDirectory};
#else
        ProcessUnix process{&mCommand, &mArguments, mWorkingDirectory};
#endif

        mExitCode = process.exitCode();
        mOutput = process.output();
    }

    //! Returns the command line arguments passed in during construction.
    [[nodiscard]] auto arguments() const noexcept -> const std::vector<std::string> &
    {
        return mArguments;
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
    [[nodiscard]] auto output() const noexcept -> const std::string &
    {
        return mOutput;
    }

    //! Returns the currently used working directory.
    [[nodiscard]] auto workingDirectory() const noexcept -> const std::string &
    {
        return mWorkingDirectory;
    }

private:
    std::string mCommand;
    std::vector<std::string> mArguments;
    std::string mOutput;
    std::string mWorkingDirectory;
    int mExitCode = 0;
};
}
