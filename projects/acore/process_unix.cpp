// clang-format off
import <unistd.h>;
import <wait.h>;
// clang-format on

namespace acore
{
class Pipe
{
public:
    Pipe()
    {
        if (pipe(mPipe) != 0)
        {
            throw std::runtime_error{"Failed to create pipe for the child process."};
        }
    }

    Pipe(const Pipe &other) = delete;

    Pipe(Pipe &&other) noexcept
    {
        *this = std::move(other);
    }

    ~Pipe()
    {
        closeRead();
        closeWrite();
    }

    auto closeRead() -> void
    {
        if (mPipe[READ] != INVALID_DESCRIPTOR)
        {
            close(mPipe[READ]);
            mPipe[READ] = INVALID_DESCRIPTOR;
        }
    }

    auto closeWrite() -> void
    {
        if (mPipe[WRITE] != INVALID_DESCRIPTOR)
        {
            close(mPipe[WRITE]);
            mPipe[WRITE] = INVALID_DESCRIPTOR;
        }
    }

    [[nodiscard]] auto readEnd() const -> int
    {
        return mPipe[READ];
    }

    [[nodiscard]] auto writeEnd() const -> int
    {
        return mPipe[WRITE];
    }

    auto operator=(const Pipe &other) -> Pipe & = delete;

    auto operator=(Pipe &&other) noexcept -> Pipe &
    {
        if (this != &other)
        {
            closeRead();
            closeWrite();
            mPipe[READ] = other.mPipe[READ];
            mPipe[WRITE] = other.mPipe[WRITE];
            other.mPipe[READ] = INVALID_DESCRIPTOR;
            other.mPipe[WRITE] = INVALID_DESCRIPTOR;
        }

        return *this;
    }

private:
    static constexpr size_t READ = 0;
    static constexpr size_t WRITE = 1;
    static constexpr int INVALID_DESCRIPTOR = -1;
    int mPipe[2] = {INVALID_DESCRIPTOR, INVALID_DESCRIPTOR};
};

class AsyncReader
{
public:
    AsyncReader(std::string *output, int readFileDescriptor) :
        mThread{[output, readFileDescriptor] {
		constexpr size_t BUFFER_SIZE = 65536;
		static char buffer[BUFFER_SIZE] = {};
		ssize_t bytesRead = 0;

		while ((bytesRead = read(readFileDescriptor, buffer, BUFFER_SIZE)) > 0)
		{
		    output->append(buffer, bytesRead);
		}
        }}
    {
    }
    
    AsyncReader(const AsyncReader&other) = delete;
    AsyncReader(AsyncReader &&other) noexcept = default;

    ~AsyncReader()
    {
        if (mThread.joinable())
        {
            mThread.join();
        }
    }
    
    auto operator=(const AsyncReader &other) -> AsyncReader & = delete;
    auto operator=(AsyncReader &&other) noexcept -> AsyncReader & = default;

private:
    std::thread mThread;
};

class ProcessUnix
{
public:
    ProcessUnix(std::string *command, std::vector<std::string> *arguments, const std::string &workingDirectory)
    {
        const pid_t pid = fork();

        if (pid == 0)
        {
            childProcess(command, arguments, workingDirectory);
        }
        else
        {
            parentProcess(pid);
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
    auto captureStdOutAndErr() -> void
    {
        mPipe.closeRead();
        dup2(mPipe.writeEnd(), STDOUT_FILENO);
        dup2(STDOUT_FILENO, STDERR_FILENO);
    }

    static auto changeDirectory(const char *directory) -> void
    {
        if (chdir(directory) == -1)
        {
            exit(errno);
        }
    }

    auto childProcess(std::string *command, std::vector<std::string> *arguments, const std::string &workingDirectory) -> void
    {
        captureStdOutAndErr();
        changeDirectory(workingDirectory.c_str());
        execv(command->c_str(), createArguments(command, arguments).data());
        std::exit(errno);
    }

    [[nodiscard]] static auto createArguments(std::string *command, std::vector<std::string> *arguments) -> std::vector<char *>
    {
        std::vector<char *> args{command->data()};
        args.reserve(arguments->size() + 2);

        for (auto &arg : *arguments)
        {
            args.push_back(arg.data());
        }

        args.push_back(nullptr);
        return args;
    }

    auto parentProcess(pid_t pid) -> void
    {
        mPipe.closeWrite();
        AsyncReader reader{&mOutput, mPipe.readEnd()};
        mExitCode = waitForFinished(pid);
    }

    [[nodiscard]] static auto waitForFinished(pid_t pid) -> int
    {
        int status = 0;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }

    Pipe mPipe;
    std::string mOutput;
    int mExitCode = 0;
};
}
