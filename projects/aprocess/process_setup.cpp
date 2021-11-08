#ifndef __clang__
export module aprocess : process_setup;
export import : environment_variable;
#endif

namespace aprocess
{
export class Process;

//! ProcessSetup
export struct ProcessSetup
{
    //! Command to run as subprocess
    std::string command;

    //! Arguments to the command
    std::vector<std::string> arguments;

    //! Environment variables to set for the
    //! process
    std::vector<EnvironmentVariable> environment;

    //! Working directory of the process
    std::string workingDirectory = std::filesystem::current_path().string();

    //! Callback invoked on process output. When
    //! not specified the stdout/stderr pipe will
    //! not be opened.
    std::function<auto(std::string_view output, Process &)->void> read{};

    //! Whether to open stdin pipe to write to the
    //! process.
    bool write = false;
};
}
