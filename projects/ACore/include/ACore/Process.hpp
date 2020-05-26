#pragma once

#include "Vector.hpp"

#include <string>

namespace acore
{
class Process
{
public:
    explicit Process(std::string command);
    Process(std::string command, std::string workingDirectory);

    [[nodiscard]] const std::string &command() const noexcept;
    [[nodiscard]] int exitCode() const noexcept;
    [[nodiscard]] const std::string &standardError() const noexcept;
    [[nodiscard]] const std::string &standardOutput() const noexcept;
    [[nodiscard]] const std::string &workingDirectory() const noexcept;

private:
    int executeProcess();

    std::string mCommand;
    std::string mError;
    std::string mOutput;
    std::string mWorkingDirectory;
    int mExitCode = 0;
};
}
