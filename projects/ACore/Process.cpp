#include "Process.hpp"

#include <tiny-process-library/process.hpp>

#include <filesystem>

namespace acore
{
Process::Process(std::string command) :
    Process{std::move(command), std::filesystem::current_path().string()}
{
}

Process::Process(std::string command, std::string workingDirectory) :
    mCommand{std::move(command)},
    mWorkingDirectory{std::move(workingDirectory)}
{
    mExitCode = executeProcess();
}

auto Process::command() const noexcept -> const std::string &
{
    return mCommand;
}

auto Process::exitCode() const noexcept -> int
{
    return mExitCode;
}

auto Process::standardError() const noexcept -> const std::string &
{
    return mError;
}

auto Process::standardOutput() const noexcept -> const std::string &
{
    return mOutput;
}

auto Process::workingDirectory() const noexcept -> const std::string &
{
    return mWorkingDirectory;
}

auto Process::executeProcess() -> int
{
    TinyProcessLib::Process process{command(),
                                    workingDirectory(),
                                    [&](const char *output, size_t len) {
                                        mOutput = std::string(output, len);
                                    },
                                    [&](const char *error, size_t len) {
                                        mError = std::string(error, len);
                                    }};
    return process.get_exit_status();
}
}
