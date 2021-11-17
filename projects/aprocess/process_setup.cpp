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
    std::string workingDirectory = std::filesystem::current_path().string();
    std::function<auto(std::string_view output)->void> read{};
    bool write = false;
    bool detached = false;
};
}
