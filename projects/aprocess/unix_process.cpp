#ifndef __clang__
module aprocess : unix_process;
import : process_setup;
import : pipe;
#    ifndef _WIN32
import<poll.h>;
import<wait.h>;
#    endif
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
        this->monitorPipe.close_on_exec();
        this->processId = ::fork(); // NOLINT(cppcoreguidelines-prefer-member-initializer)

        if (this->processId == 0)
        {
            this->child_process();
        }
        else
        {
            this->parent_process();
        }
    }

    ~UnixProcess()
    {
        if (!this->setup->detached && this->is_running())
        {
            this->kill();
        }
    }

    UnixProcess(const UnixProcess &other) = delete;
    UnixProcess(UnixProcess &&other) noexcept = default;

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

        return WEXITSTATUS(this->status); // NOLINT(hicpp-signed-bitwise)
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
        ::kill(this->processId, SIGKILL);
    }

    [[nodiscard]] auto pid() const noexcept -> std::int64_t
    {
        return this->processId;
    }

    [[nodiscard]] auto read(std::chrono::milliseconds timeout) -> std::string
    {
        if (this->wait_for_data_to_read(timeout))
        {
            return this->do_read();
        }

        return {};
    }

    auto terminate() const -> void
    {
        ::kill(-this->processId, SIGINT);
        ::kill(-this->processId, SIGTERM);
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
        if (!this->do_write(message))
        {
            throw std::runtime_error{"Failed to write to process: " + std::to_string(errno)};
        }
    }

    auto operator=(const UnixProcess &other) -> UnixProcess & = delete;
    auto operator=(UnixProcess &&other) noexcept -> UnixProcess & = default;

private:
    auto change_directory() -> void
    {
        if (::chdir(this->setup->workingDirectory.c_str()) == -1)
        {
            this->signal_error();
        }
    }

    auto child_process() -> void
    {
        UnixProcess::reset_ignore_sigpipe_signal();
        this->setup_child_if_detached();
        this->setup_child_monitor_pipe();
        this->setup_child_read_pipe();
        this->setup_child_write_pipe();
        this->change_directory();
        this->exec();
    }

    auto clone_existing_environment() -> void
    {
        char **env = ::environ;

        while (*env != nullptr)
        {
            this->environment.emplace_back(*env);
            ++env; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
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
        this->set_new_environment_variables();
        this->clone_existing_environment();
        return this->create_environment_pointers();
    }

    [[nodiscard]] auto create_environment_pointers() -> std::vector<char *>
    {
        std::vector<char *> envPtr;
        envPtr.reserve(this->environment.size());

        for (std::string &var : this->environment)
        {
            envPtr.push_back(var.data());
        }

        envPtr.push_back(nullptr);

        return envPtr;
    }

    [[nodiscard]] auto do_read() -> std::string
    {
        constexpr std::size_t bufferSize = 65'536;
        std::string buffer(bufferSize, char{});
        std::size_t bytesRead = 0;
        std::stringstream output;

        do
        {
            bytesRead = ::read(this->readPipe.read_end(), buffer.data(), buffer.size());
            output << std::string_view{buffer.data(), bytesRead};
        }
        while (bytesRead == buffer.size());

        return output.str();
    }

    [[nodiscard]] auto do_write(const std::string &message) -> bool
    {
        const ::ssize_t result = ::write(this->writePipe.write_end(), message.data(), message.size());
        return result != -1 && static_cast<std::size_t>(result) == message.size();
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

        this->signal_error();
    }

    static auto ignore_sigpipe_signal() -> void
    {
        ::signal(SIGPIPE, SIG_IGN); //NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    }

    [[nodiscard]] auto wait_for_data_to_read(std::chrono::milliseconds timeout) -> bool
    {
        std::array<::pollfd, 1> fds{
            ::pollfd{
                .fd = this->readPipe.read_end(),
                .events = POLLIN}};

        return ::poll(fds.data(), 1, static_cast<int>(timeout.count())) == 1;
    }

    auto parent_process() -> void
    {
        UnixProcess::ignore_sigpipe_signal();
        this->setup_parent_monitor_pipe();
        this->setup_parent_read_pipe();
        this->setup_parent_write_pipe();
        this->wait_for_started();
    }

    static auto reset_ignore_sigpipe_signal() -> void
    {
        ::signal(SIGPIPE, SIG_DFL);
    }

    auto set_new_environment_variables() -> void
    {
        for (const EnvironmentVariable &envVar : this->setup->environment)
        {
            this->environment.push_back(envVar.name + '=' + envVar.value);
        }
    }

    auto setup_child_if_detached() -> void
    {
        if (this->setup->detached)
        {
            ::setsid();
        }
    }

    auto setup_child_monitor_pipe() -> void
    {
        this->monitorPipe.close_read();
    }

    auto setup_child_read_pipe() -> void
    {
        this->readPipe.close_read();
        ::dup2(this->readPipe.write_end(), STDOUT_FILENO);
        ::dup2(STDOUT_FILENO, STDERR_FILENO);
    }

    auto setup_child_write_pipe() -> void
    {
        this->writePipe.close_write();
        ::dup2(this->writePipe.read_end(), STDIN_FILENO);
    }

    auto setup_parent_monitor_pipe() -> void
    {
        this->monitorPipe.close_write();
    }

    auto setup_parent_read_pipe() -> void
    {
        this->readPipe.close_write();
    }

    auto setup_parent_write_pipe() -> void
    {
        this->writePipe.close_read();
    }

    auto signal_error() -> void
    {
        const int e = errno;
        ::write(this->monitorPipe.write_end(), &e, sizeof(e));
        std::mutex mutex;
        const std::lock_guard<std::mutex> lock(mutex);
        std::exit(e); // NOLINT(concurrency-mt-unsafe)
    }

    auto wait_for_started() -> void
    {
        int e = 0;

        if (::read(this->monitorPipe.read_end(), &e, sizeof(e)) != 0)
        {
            throw std::runtime_error{"Failed to create process: " + std::to_string(e)};
        }
    }

    ProcessSetup *setup = nullptr;
    int processId = 0;
    int status = 0;
    std::vector<std::string> environment;
    Pipe readPipe;
    Pipe writePipe;
    Pipe monitorPipe;
    bool running = true;
};
}
#endif
