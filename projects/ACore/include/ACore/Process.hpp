// Copyright 2020 Michael Vlach
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ACORE_PROCESS_HPP
#define ACORE_PROCESS_HPP

#include "ACoreModule.hpp"

#include <string>

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
class Process
{
public:
    //! Constructs the Process object and executes \a command.
    explicit Process(std::string command);

    //! Constructs the Process object and executes \a command
    //! as if run from the \a workingDirectory.
    Process(std::string command, std::string workingDirectory);

    //! Returns the command passed in during construction.
    [[nodiscard]] auto command() const noexcept -> const std::string &;

    //! Returns the return value (exit code) of the run command.
    [[nodiscard]] auto exitCode() const noexcept -> int;

    //! Returns the standard error contents of the run command.
    [[nodiscard]] auto standardError() const noexcept -> const std::string &;

    //! Returns teh standard output contents of the run command.
    [[nodiscard]] auto standardOutput() const noexcept -> const std::string &;

    //! Returns the currently used working directory.
    [[nodiscard]] auto workingDirectory() const noexcept -> const std::string &;

private:
    [[nodiscard]] auto executeProcess() -> int;

    std::string mCommand;
    std::string mError;
    std::string mOutput;
    std::string mWorkingDirectory;
    int mExitCode = 0;
};
}

#endif
