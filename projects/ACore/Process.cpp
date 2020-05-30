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
