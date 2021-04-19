// clang-format off
import <unistd.h>;
import <sys/wait.h>;
// clang-format on

namespace acore
{
class ProcessUnix
{
public:
    ProcessUnix(const std::string &command, const std::vector<std::string> &arguments, const std::string &workingDirectory)
    {
        auto pid = fork();
        auto args = createArguments(arguments);
        chdir(workingDirectory.c_str());
        execv(command.c_str(), args.data());
        waitpid(pid, &mStatus, 0);
    }

    [[nodiscard]] auto exitCode() const -> int
    {
        return mStatus;
    }

    [[nodiscard]] auto output() const -> std::string
    {
        return {};
    }

private:
    [[nodiscard]] static auto createArguments(const std::string &command, const std::vector<std::string> &arguments) -> std::vector<const char *>
    {
        std::vector<const char *> args{};
        args.reserve(arguments.size() + 2);
        args.push_back(command.c_str());

        for (const auto &arg : arguments)
        {
            args.push_back(arg.c_str());
        }

        args.push_back(nullptr);
        return args;
    }

    int mStatus = 0;
};
}