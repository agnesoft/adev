#ifndef __clang__
module aprocess : process_setup;
import : environment_variable;
#endif

namespace aprocess
{
//! \private
struct ProcessSetup
{
    std::string command;
    std::vector<std::string> arguments;
    std::vector<EnvironmentVariable> environment;
    std::filesystem::path workingDirectory = std::filesystem::current_path();
    bool detached = false;
};
}
