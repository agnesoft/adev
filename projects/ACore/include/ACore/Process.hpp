#ifndef ACORE_PROCESS_HPP
#define ACORE_PROCESS_HPP

#include <string>

namespace acore
{
class Process
{
public:
    explicit Process(std::string command);
    Process(std::string command, std::string workingDirectory);

    [[nodiscard]] auto command() const noexcept -> const std::string &;
    [[nodiscard]] auto exitCode() const noexcept -> int;
    [[nodiscard]] auto standardError() const noexcept -> const std::string &;
    [[nodiscard]] auto standardOutput() const noexcept -> const std::string &;
    [[nodiscard]] auto workingDirectory() const noexcept -> const std::string &;

private:
    int executeProcess();

    std::string mCommand;
    std::string mError;
    std::string mOutput;
    std::string mWorkingDirectory;
    int mExitCode = 0;
};
}

#endif
