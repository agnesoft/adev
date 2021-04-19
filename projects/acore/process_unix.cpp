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
        int outputPipe[2] = {};
        
        if (pipe(outputPipe) != 0)
        {
            throw std::runtime_error{"Failed to create pipe for the child process."};
        }
        
        if (pid == 0)
        {           
            if (chdir(workingDirectory.c_str()) == -1)
            {
                exit(errno);            
            }
            
            close(outputPipe[0]);
            dup2(outputPipe[1], STDOUT_FILENO);
            //dup2(STDOUT_FILENO, STDERR_FILENO);
            
            auto args = createArguments(command, arguments);
            execv(command->c_str(), args.data());
            std::exit(errno);
        }
        else
        {
            close(outputPipe[1]);
            FILE *stream = fdopen(outputPipe[0], "r");
            int status = 0;
            waitpid(pid, &status, 0);
            mExitCode = WEXITSTATUS(status);

            constexpr size_t BUFFER_SIZE = 65536;
            static char buffer[BUFFER_SIZE] = {};
//            ssize_t bytesRead = 0;
            int fileDescriptor = fileno(stream);
            const ssize_t bytesRead = read(fileDescriptor, buffer, BUFFER_SIZE);
            
            mOutput.append(buffer, bytesRead);
            
            std::cout << "READ: " << bytesRead << '\n';

            fclose(stream);
            close(outputPipe[0]);
            close(outputPipe[1]);
        }
    }

    [[nodiscard]] auto exitCode() const -> int
    {
        return mExitCode;
    }

    [[nodiscard]] auto output() const -> std::string
    {
        return mOutput;
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

    std::string mOutput;
    int mExitCode = 0;
};
}
