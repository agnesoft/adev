#ifndef __clang__
export module aprocess : process;
export import : process_setup;
import : windows_process;
#endif

namespace aprocess
{
//! The Process is a cross platform representation
//! of system process. The new process is spawned
//! in the constructor and started asynchronously,
//! similar to the `std::thread`. The arguments
//! are not treated in any way before they are
//! passed to the underlying operating system
//! functions.
//!
//! You can use the blocking wait() method to wait
//! for the process to finish. The process can be
//! stopped with terminate(), which will allow it
//! to exit gracefully, or with kill() that will
//! forcefully terminate it. You can check the
//! status of the process with is_running() and
//! detach() from the process so that it can
//! outlive its parent. If the process is not
//! detached the destructor will always call
//! kill() if the process is still running to
//! forcefully stop it.
//!
//! In order to capture the output call read().
//! Calling it after successful wait(),
//! terminate() or kill() (e.g. after the process
//! is stopped) will always return all of the
//! remaining output:
//!
//! \snippet aprocess/test/examples.cpp
//! [synchronous process]
//!
//! NOTE: The `stdout` and `stderr` are combined
//! in order to make the Process class consistent
//! across platforms. Many applications ignore
//! this distinction or use it inconsistently.
//!
//! You can achieve real time output of the
//! process with a busy loop (consider offloading
//! it to another thread as it may deadlock your
//! application if the spawned process never
//! finishes):
//!
//! \snippet aprocess/test/examples.cpp [real time
//! output]
//!
//! Finally you can send input into the process
//! with write():
//!
//! \snippet aprocess/test/examples.cpp [input]
export class Process
{
public:
    //! The Builder is a wrapper type for process
    //! builder functions. It cannot be
    //! constructed directly but rather via
    //! ::aprocess::process() function that starts
    //! the builder process.
    class Builder
    {
    public:
        //! Adds `args` to the process arguments.
        [[nodiscard]] auto arguments(const std::vector<std::string> &args) -> Builder &
        {
            this->setup.arguments.insert(this->setup.arguments.end(), args.begin(), args.end());
            return *this;
        }

        //! Adds single `argument to the process
        //! arguments.
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

        //! Adds `variables` to the process
        //! environment variables.
        [[nodiscard]] auto environment(const std::vector<EnvironmentVariable> &variables) -> Builder &
        {
            this->setup.environment.insert(this->setup.environment.end(), variables.begin(), variables.end());
            return *this;
        }

        //! Adds `variable` to the process
        //! environment variables.
        [[nodiscard]] auto env(EnvironmentVariable variable) -> Builder &
        {
            this->setup.environment.emplace_back(std::move(variable));
            return *this;
        }

        //! Sets the process to use GUI
        //! application specific behaviors.
        //!
        //! On Windows this will set the right
        //! flags for CreateProcess and send
        //! WM_CLOSE on call to
        //! Process::terminate().
        [[nodiscard]] auto gui() -> Builder &
        {
            this->setup.gui = true;
            return *this;
        }

        //! Sets the `callback` that is invoked on
        //! any output of the process. If the
        //! callback is not set the process will
        //! not open the read pipe of the process.
        [[nodiscard]] auto read(std::function<auto(std::string_view output)->void> callback) -> Builder &
        {
            this->setup.read = callback;
            return *this;
        }

        //! Waits for `timeout` for the process to
        //! finish. Throws an exception if the
        //! wait times out. Returns the stopped
        //! Process object if it succeeds.
        auto wait(std::chrono::milliseconds timeout) -> Process
        {
            Process proc{std::move(this->setup)};
            proc.wait(timeout);
            return proc;
        }

        //! Sets working directory of the process
        //! to `directory`.
        [[nodiscard]] auto working_directory(std::string directory) -> Builder &
        {
            this->setup.workingDirectory = std::move(directory);
            return *this;
        }

        //! Enables writing to the process. If
        //! this is not enabled the process will
        //! not open the write pipe.
        [[nodiscard]] auto write() -> Builder &
        {
            this->setup.write = true;
            return *this;
        }

        operator Process()
        {
            return Process{std::move(this->setup)};
        }

    private:
        Builder() = default;

        friend class Process;
        friend auto process() -> Process::Builder;

        ProcessSetup setup;
    };

    //! Deleted copy constructor.
    Process(const Process &other) = delete;

    //! Move constructor.
    Process(Process &&other) noexcept = default;

    //! Destroys the process object. If the
    //! process is still running it is first
    //! killed by calling kill().
    ~Process() = default;

    //! Returns the exit code of the process when
    //! it finishes otherwise returns `0`.
    [[nodiscard]] auto exit_code() const -> int
    {
        return this->process.exit_code();
    }

    //! Returns `true` if the process is still
    //! running or `false` otherwise.
    [[nodiscard]] auto is_running() const -> bool
    {
        return this->process.is_running();
    }

    //! Sends `SIGKILL` to the process.
    auto kill() -> void
    {
        this->process.kill();
    }

    //! Returns the OS process identifier (PID).
    //!
    //! \warning The PID is valid only during the
    //! process runtime and it can be reused by
    //! another process once the original stops.
    [[nodiscard]] auto pid() const noexcept -> std::int64_t
    {
        return this->process.pid();
    }

    //! Returns the process setup that was used
    //! when constructing the process.
    [[nodiscard]] auto process_setup() const noexcept -> const ProcessSetup &
    {
        return this->processSetup;
    }

    //! Sends `SIGTERM` to the process. You should
    //! follow this by call to wait() to allow the
    //! process to gracefully exit.
    auto terminate() -> void
    {
        this->process.terminate();
    }

    //! Waits for `timeout` for the process to
    //! finish. Throws an exception if the wait
    //! times out.
    auto wait(std::chrono::milliseconds timeout) -> void
    {
        return this->process.wait(timeout);
    }

    //! Writes `message` to the process.
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
        processSetup{std::move(setup)},
        process{this->processSetup}
    {
    }

private:
    ProcessSetup processSetup;
#ifdef _WIN32
    WindowsProcess process;
#else

#endif
};

//! Starts the process composition.
export [[nodiscard]] auto process() -> Process::Builder
{
    return Process::Builder{};
}
}