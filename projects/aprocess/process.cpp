#ifndef __clang__
export module aprocess : process;
export import : environment_variable;
import : unix_process;
import : windows_process;
#endif

namespace aprocess
{
//! The Process is a cross platform representation
//! of system process. The new process is spawned
//! on construction and is started asynchronously
//! similar to the `std::thread`. To create a new
//! process start the builder sequence via
//! create_process() function.
//!
//! You can use the blocking wait() method to wait
//! for the process to finish. The process can be
//! stopped with terminate(), which will allow it
//! to exit gracefully, or with kill() that will
//! forcefully terminate it. You can check the
//! status of the process with is_running(). You
//! can creat the process `detached` so that it
//! can outlive its parent process and the Process
//! object. If the process is not detached the
//! destructor will always call kill() if the
//! process is still running to forcefully stop
//! it.
//!
//! **NOTE:** The Process blocks SIGPIPE signal on
//! Linux/Unix systems in order to handle error
//! states when communicating with the spawned
//! process directly. This is usually the desired
//! behavior. This setting is however not
//! inherited by spawned process.
//!
//! In order to capture the output call read(). If
//! you call it after the process is stopped all
//! of its remaining output is returned (or all
//! output if read() was not called).
//!
//! Immediately awaited process with arguments and
//! reading the output:
//!
//! \snippet aprocess/test/examples.cpp [process]
//!
//! **NOTE:** The `stdout` and `stderr` are
//! combined in order to make the Process class
//! consistent across platforms. Many applications
//! ignore this distinction or use it
//! inconsistently.
//!
//! Running process and capturing the output in
//! "real-time":
//!
//! \snippet aprocess/test/examples.cpp [output]
//!
//! Writing to the process with write():
//!
//! \snippet aprocess/test/examples.cpp [input]
//!
//! **NOTE:** Depending on the implementation of
//! the run program you may need to append the
//! delimiter character for the message to be
//! correctly read on the other side. For example
//! when reading with `std::cin` the default
//! delimiter character is `\n`.
export class Process
{
public:
    //! The Builder is a type for process builder
    //! functions. It cannot be constructed
    //! directly but rather via create_process()
    //! function that starts the builder process.
    class Builder
    {
    public:
        //! Adds `args` to the process arguments.
        //! The arguments are not treated in any
        //! way before they are passed to the
        //! underlying operating system functions.
        [[nodiscard]] auto arguments(const std::vector<std::string> &args) -> Builder &
        {
            this->setup.arguments.insert(this->setup.arguments.end(), args.begin(), args.end());
            return *this;
        }

        //! Adds single `argument` to the process
        //! arguments. The arguments are not
        //! treated in any way before they are
        //! passed to the underlying operating
        //! system functions.
        [[nodiscard]] auto arg(std::string argument) -> Builder &
        {
            this->setup.arguments.emplace_back(std::move(argument));
            return *this;
        }

        //! Sets the process command to `cmd` that
        //! is the path of the executable to run.
        [[nodiscard]] auto command(std::string cmd) -> Builder &
        {
            this->setup.command = std::move(cmd);
            return *this;
        }

        //! Starts the process detached meaning it
        //! can outlive its Process object.
        [[nodiscard]] auto detached() -> Builder &
        {
            this->setup.detached = true;
            return *this;
        }

        //! Prepends `variables` to the process
        //! environment variables.
        [[nodiscard]] auto environment(const std::vector<EnvironmentVariable> &variables) -> Builder &
        {
            this->setup.environment.insert(this->setup.environment.end(), variables.begin(), variables.end());
            return *this;
        }

        //! Prepends `variable` to the process
        //! environment variables.
        [[nodiscard]] auto env(EnvironmentVariable variable) -> Builder &
        {
            this->setup.environment.emplace_back(std::move(variable));
            return *this;
        }

        //! Waits for `timeout` for the process to
        //! finish. Throws an `std::runtime_error`
        //! if the wait times out. Returns the
        //! stopped Process object if it succeeds.
        auto wait(std::chrono::milliseconds timeout) -> Process
        {
            Process proc{std::move(this->setup)};
            proc.wait(timeout);
            return proc;
        }

        //! Sets working directory of the process
        //! to `directory`.
        [[nodiscard]] auto working_directory(std::filesystem::path directory) -> Builder &
        {
            this->setup.workingDirectory = std::move(directory);
            return *this;
        }

        //! Implicitly converts the Builder to the
        //! Process object by moving from itself.
        //! The Builder object is unusable
        //! afterwards.
        operator Process()
        {
            return Process{std::move(this->setup)};
        }

    private:
        ProcessSetup setup;
    };

    //! Deleted copy constructor.
    Process(const Process &other) = delete;

    //! Move constructor.
    Process(Process &&other) noexcept = default;

    //! Destroys the process object. If the
    //! process is still running it is first
    //! killed by calling kill() unless the
    //! process was created detached.
    ~Process() = default;

    //! Returns the arguments that were used to
    //! create the process.
    [[nodiscard]] auto arguments() const noexcept -> const std::vector<std::string> &
    {
        return this->setup.arguments;
    }

    //! Returns the command that was used to
    //! create the process.
    [[nodiscard]] auto command() const noexcept -> const std::string &
    {
        return this->setup.command;
    }

    //! Returns `true` if the process has been
    //! created detached (capable of outliving the
    //! Process object) or `false` otherwise.
    [[nodiscard]] auto detached() const noexcept -> bool
    {
        return this->setup.detached;
    }

    //! Returns the extra environment variables
    //! that were used to create the process.
    [[nodiscard]] auto environment() const noexcept -> const std::vector<EnvironmentVariable> &
    {
        return this->setup.environment;
    }

    //! Returns the exit code of the process when
    //! it finishes otherwise returns `0`.
    [[nodiscard]] auto exit_code() -> int
    {
        return this->process.exit_code();
    }

    //! Returns `true` if the process is still
    //! running or `false` otherwise.
    [[nodiscard]] auto is_running() -> bool
    {
        return this->process.is_running();
    }

    //! Forcefully kills the process by sending
    //! `SIGKILL` on Unix and calling
    //! `TerminateProcess` on Windows.
    auto kill() -> void
    {
        this->process.kill();
    }

    //! Returns the OS process identifier (PID).
    //!
    //! \warning The PID is valid only during the
    //! process runtime and it can be reused by
    //! another process once the original stops.
    //! Be careful when using this value.
    [[nodiscard]] auto pid() const noexcept -> std::int64_t
    {
        return this->process.pid();
    }

    //! Reads the output of the process since the
    //! previous call to read() or empty string if
    //! there is no output. If the process is not
    //! running it reads the remaining output if
    //! any. This function does not block and does
    //! not wait for output if there is none. It
    //! is most suitable to be called after the
    //! process finished.
    [[nodiscard]] auto read() -> std::string
    {
        return read(std::chrono::milliseconds{0});
    }

    //! Reads the output of the process since the
    //! previous call to read() or empty string if
    //! there is no output. This function waits up
    //! to `timeout` for output to appear.
    [[nodiscard]] auto read(std::chrono::milliseconds timeout) -> std::string
    {
        return process.read(timeout);
    }

    //! Sends `SIGINT and `SIGTERM` to the process
    //! on Unix or `WM_CLOSE` and `CTRL_C_EVENT`
    //! on Windows. You should follow this by call
    //! to wait() to allow the process to
    //! gracefully exit.
    auto terminate() -> void
    {
        this->process.terminate();
    }

    //! Waits for `timeout` for the process to
    //! finish. Throws an `std::runtime_error` if
    //! the wait times out.
    auto wait(std::chrono::milliseconds timeout) -> void
    {
        return this->process.wait(timeout);
    }

    //! Returns the working directory that was
    //! used to create the process.
    [[nodiscard]] auto working_directory() const noexcept -> const std::filesystem::path &
    {
        return this->setup.workingDirectory;
    }

    //! Writes `message` to the process. You may
    //! need to append or write separately the
    //! delimiter character for the message to be
    //! correctly received on the other end
    //! depending on the implementation of the run
    //! application.
    //!
    //! E.g. The end-of-line `\n` character when
    //! the application reads using the default
    //! settings and `std::cin`.
    //!
    //! When the message could not be written in
    //! full `std::runtime_error` is thrown.
    //!
    //! \warning If the run program does not read
    //! on the STDIN pipe this function might
    //! block until the program ends and the pipe
    //! is closed.
    auto write(const std::string &message) -> void
    {
        this->process.write(message);
    }

    //! Deleted copy assignment.
    auto operator=(const Process &other) -> Process & = delete;

    //! Move assignment.
    auto operator=(Process &&other) noexcept -> Process & = default;

private:
    explicit Process(ProcessSetup setup) :
        setup{std::move(setup)},
        process{this->setup}
    {
    }

    ProcessSetup setup;
#ifdef _WIN32
    WindowsProcess process;
#else
    UnixProcess process;
#endif
};

//! Starts the process composition.
//! \related Process
export [[nodiscard]] auto create_process() -> Process::Builder
{
    return Process::Builder{};
}
}