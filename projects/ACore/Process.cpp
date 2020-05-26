#include "Process.hpp"

#include <tiny-process-library/process.cpp>
#include <tiny-process-library/process.hpp>
#ifdef _WIN32
#    include <tiny-process-library/process_win.cpp>
#else
#    include <tiny-process-library/process_unix.cpp>
#endif

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

const std::string &Process::command() const noexcept
{
    return mCommand;
}

int Process::exitCode() const noexcept
{
    return mExitCode;
}

const std::string &Process::standardError() const noexcept
{
    return mError;
}

const std::string &Process::standardOutput() const noexcept
{
    return mOutput;
}

const std::string &Process::workingDirectory() const noexcept
{
    return mWorkingDirectory;
}

int Process::executeProcess()
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
