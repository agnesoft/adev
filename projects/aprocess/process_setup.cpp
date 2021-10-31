#ifndef __clang__
export module aprocess : process_setup;
export import : environment_variable;
#endif

namespace aprocess
{
//! ProcessSetup
export struct ProcessSetup
{
    //! Command to run as subprocess
    std::string command;

    //! Arguments to the command
    std::vector<std::string> arguments;

    //! Environment variables to set for the process
    std::vector<EnvironmentVariable> environment;

    //! Working directory of the process
    std::string workingDirectory = std::filesystem::current_path().string();
};
}
