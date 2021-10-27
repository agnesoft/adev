export module aprocess;

export import astl;

#ifndef __clang__

#else
// clang-format off

// clang-format on
#endif

namespace aprocess
{
//! EnvironmentVariable
export struct EnvironmentVariable
{
    //! Name of the environment variable
    std::string name;

    //! Value of environment variable
    std::string value;
};

//! ProcessSetup
export struct ProcessSetup
{
    //! Command to run as subprocess
    std::string command;

    //! Arguments to the command
    std::vector<std::string> arguments;

    //! Environment variables to set for the process
    std::vector<EnvironmentVariable> environment;

    //! Working directory of the process
    std::string workingDirectory = std::filesystem::current_path().string();
};

//! The Process is a cross platform representation
//! of system process. The new process is spawned
//! in the constructor and started asynchronously,
//! similar to the `std::thread`. The arguments
//! are not treated in any way before they are
//! passed to the underlying operating system
//! functions.
//!
//! You can use the blocking wait() method to wait
//! for the process to finish. It optionally takes
//! the `std::chrono::duration` argument as a
//! timeout. The process can be stopped with
//! terminate(), which will allow it to exit
//! gracefully, or with kill() that will
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
//! \snippet aprocess/test/examples.cpp [synchronous process]
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
//! \snippet aprocess/test/examples.cpp [real time output]
//!
//! Finally you can send input into the process
//! with write():
//!
//! \snippet aprocess/test/examples.cpp [input]
export class Process
{
public:
    //! Starts the process defined by `setup`.
    explicit Process(ProcessSetup setup) :
        processSetup{std::move(setup)}
    {
    }

    Process(const Process &other) = delete;

    //! Move constructor.
    Process(Process &&other) noexcept = default;

    //! Destroys the process object. If the
    //! process is still running it is first
    //! killed by calling kill().
    ~Process() = default;

    //! Detaches from the process so it can
    //! outlive the parent process.
    auto detach() -> void
    {
        static_cast<void>(this->processSetup);
    }

    //! Returns the exit code of the process when
    //! it finishes otherwise returns `0`.
    [[nodiscard]] auto exit_code() const -> int
    {
        static_cast<void>(this->processSetup);
        return 0;
    }

    //! Returns `true` if the process is still
    //! running or `false` otherwise.
    [[nodiscard]] auto is_running() const -> bool
    {
        static_cast<void>(this->processSetup);
        return false;
    }

    //! Sends `SIGKILL` to the process.
    auto kill() -> void
    {
        static_cast<void>(this->processSetup);
    }

    //! Reads the output of the process since the
    //! previous call to read(). If it is called
    //! after the process exited it will read rest
    //! of the output if there is any.
    [[nodiscard]] auto read() -> std::string
    {
        static_cast<void>(this->processSetup);
        return {};
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
        static_cast<void>(this->processSetup);
    }

    //! Waits indefinitely for the process to
    //! finish.
    auto wait() -> void
    {
        static_cast<void>(this->wait(std::chrono::seconds::zero()));
    }

    //! Waits for `timeout` for the process to
    //! finish. Returns `true` if the process
    //! finished in time or `false` if it timed
    //! out.
    [[nodiscard]] auto wait([[maybe_unused]] std::chrono::seconds timeout) -> bool
    {
        static_cast<void>(this->processSetup);
        return false;
    }

    //! Writes `message` to the process.
    auto write([[maybe_unused]] const std::string &message) -> void
    {
        static_cast<void>(this->processSetup);
    }

    auto operator=(const Process &other) -> Process & = delete;

    //! Move assignment.
    auto operator=(Process &&other) noexcept -> Process & = default;

private:
    ProcessSetup processSetup;
};
}
