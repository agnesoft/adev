// clang-format off
import <unistd.h>;
import <wait.h>;
// clang-format on

namespace acore
{
class ProcessUnix
{
public:
    ProcessUnix(std::string *command, std::vector<std::string> *arguments, const std::string &workingDirectory)
    {
        auto pid = fork();
        
        if (pid == 0)
        {           
            if (chdir(workingDirectory.c_str()) == -1)
            {
                exit(errno);            
            }
            
            auto args = createArguments(command, arguments);
            execv(command->c_str(), args.data());
            std::exit(errno);
        }
        else
        {
            int status = 0;
            waitpid(pid, &status, 0);
            mExitCode = WEXITSTATUS(status);
        }
    }

    [[nodiscard]] auto exitCode() const -> int
    {
        return mExitCode;
    }

    [[nodiscard]] auto output() const -> std::string
    {
        return {};
    }

private:
    [[nodiscard]] static auto createArguments(std::string *command, std::vector<std::string> *arguments) -> std::vector<char *>
    {
        std::vector<char *> args{};
        args.reserve(arguments->size() + 2);
        args.push_back(command->data());

        for (auto &arg : *arguments)
        {
            args.push_back(arg.data());
        }

        args.push_back(nullptr);
        return args;
    }

    int mExitCode = 0;
};
}
