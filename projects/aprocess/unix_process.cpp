#ifndef __clang__
module aprocess : unix_process;
import : process_setup;
import : pipe;
import : async_reader;
//import<signal.h>;
import<wait.h>;
#endif

#ifndef _WIN32
namespace aprocess
{
//! \private
class UnixProcess
{
public:
    explicit UnixProcess(ProcessSetup &setup) :
        setup{&setup}
    {
        this->processId = ::fork();

        if (this->processId == 0)
        {
            this->child_process();
        }
        else
        {
            this->parent_process();
        }
    }

    [[nodiscard]] auto exit_code() -> int
    {
        if (this->running)
        {
            const int code = ::waitpid(this->processId, &this->status, WNOHANG);
            
            if (code == this->processId)
            {
                this->running = false;
            }
            else if (code == -1 && errno != ECHILD)
            {
                throw std::runtime_error{"Failed to get exit code of process: " + std::to_string(errno)};
            }
        }
        
        return this->status;
    }

    [[nodiscard]] auto is_running() -> bool
    {
        if (this->running)
        {
            this->running = (::waitpid(this->processId, &this->status, WNOHANG) == 0);
        }

        return this->running;
    }

    auto kill() const -> void
    {
        ::kill(-this->processId, SIGTERM);
    }

    [[nodiscard]] auto pid() const noexcept -> std::int64_t
    {
        return this->processId;
    }

    auto terminate() const -> void
    {
        ::kill(-this->processId, SIGINT);
    }

    auto wait(std::chrono::milliseconds timeout) -> void
    {
        if (this->running)
        {
            const auto end = std::chrono::system_clock::now() + timeout;

            while (std::chrono::system_clock::now() < end && this->is_running())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
            }

            if (this->running)
            {
                throw std::runtime_error{"Wait for process timed out."};
            }
        }
    }

    auto write(const std::string &message) -> void
    {
        if (!this->setup->write)
        {
            throw std::logic_error{"The process does not have stdin pipe open."};
        }

        if (::write(this->writePipe.write_end(), message.data(), message.size()) == -1)
        {
            throw std::runtime_error{"Failed to write to process: " + std::to_string(errno)};
        }
    }

private:
    auto change_directory() -> void
    {
        if (::chdir(this->setup->workingDirectory.c_str()) == -1)
        {
            std::exit(errno);
        }
    }

    auto child_process() -> void
    {
        this->setup_child_read_pipe();
        this->setup_child_write_pipe();
        this->change_directory();
        this->exec();
        std::exit(errno);
    }

    [[nodiscard]] auto create_arguments() -> std::vector<char *>
    {
        std::vector<char *> args;
        args.reserve(this->setup->arguments.size() + 2);
        args.push_back(this->setup->command.data());

        for (std::string &arg : this->setup->arguments)
        {
            args.push_back(arg.data());
        }

        args.push_back(nullptr);
        return args;
    }

    [[nodiscard]] auto create_environment() -> std::vector<char *>
    {
        for (const EnvironmentVariable &envVar : this->setup->environment)
        {
            this->environment.push_back(envVar.name + '=' + envVar.value);
        }

        char **env = ::environ;

        while (*env != nullptr)
        {
            this->environment.emplace_back(*env);
            env += this->environment.back().size() + 1; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }

        std::vector<char *> envPtr;
        envPtr.reserve(this->environment.size());

        for (std::string &var : this->environment)
        {
            envPtr.push_back(var.data());
        }

        envPtr.push_back(nullptr);
        return envPtr;
    }

    auto exec() -> void
    {
        if (this->setup->environment.empty())
        {
            ::execvp(this->setup->command.c_str(), this->create_arguments().data());
        }
        else
        {
            ::execvpe(this->setup->command.c_str(), this->create_arguments().data(), this->create_environment().data());
        }

        std::exit(errno);
    }

    auto parent_process() -> void
    {
        this->setup_parent_read_pipe();
        this->setup_parent_write_pipe();
    }

    auto setup_child_read_pipe() -> void
    {
        this->readPipe.close_read();

        if (this->setup->read)
        {
            ::dup2(this->readPipe.write_end(), STDOUT_FILENO);
            ::dup2(STDOUT_FILENO, STDERR_FILENO);
        }
        else
        {
            this->readPipe.close_write();
        }
    }

    auto setup_child_write_pipe() -> void
    {
        this->writePipe.close_write();

        if (this->setup->write)
        {
            ::dup2(this->writePipe.read_end(), STDIN_FILENO);
        }
        else
        {
            this->writePipe.close_read();
        }
    }

    auto setup_parent_read_pipe() -> void
    {
        this->readPipe.close_write();

        if (this->setup->read)
        {
            this->asyncReader = std::make_unique<AsyncReader>(this->readPipe.read_end(), *this->setup);
        }
        else
        {
            this->readPipe.close_read();
        }
    }

    auto setup_parent_write_pipe() -> void
    {
        this->writePipe.close_read();

        if (!this->setup->write)
        {
            this->writePipe.close_write();
        }
    }

    [[nodiscard]] static auto waitForFinished(pid_t pid) -> int
    {
        int status = 0;
        ::waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }

    ProcessSetup *setup = nullptr;
    int processId = 0;
    int status = 0;
    std::vector<std::string> environment;
    std::unique_ptr<AsyncReader> asyncReader;
    Pipe readPipe;
    Pipe writePipe;
    bool running = true;
};
}
#endif
