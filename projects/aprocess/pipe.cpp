#ifndef __clang__
module aprocess : pipe;
import astl;
#    ifndef _WIN32
import<fcntl.h>;
import<unistd.h>;
#    endif
#endif

#ifndef _WIN32
class Pipe
{
public:
    Pipe()
    {
        if (::pipe(this->pipes) != 0)
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
        this->close_read();
        this->close_write();
    }

    auto close_on_exec() -> void
    {
        ::fcntl(this->pipes[Pipe::READ], F_SETFD, FD_CLOEXEC);
        ::fcntl(this->pipes[Pipe::WRITE], F_SETFD, FD_CLOEXEC);
    }

    auto close_read() -> void
    {
        if (this->pipes[Pipe::READ] != Pipe::INVALID_DESCRIPTOR)
        {
            ::close(this->pipes[Pipe::READ]);
            this->pipes[Pipe::READ] = Pipe::INVALID_DESCRIPTOR;
        }
    }

    auto close_write() -> void
    {
        if (this->pipes[Pipe::WRITE] != Pipe::INVALID_DESCRIPTOR)
        {
            ::close(this->pipes[Pipe::WRITE]);
            this->pipes[Pipe::WRITE] = Pipe::INVALID_DESCRIPTOR;
        }
    }

    [[nodiscard]] auto read_end() const -> int
    {
        return this->pipes[Pipe::READ];
    }

    [[nodiscard]] auto write_end() const -> int
    {
        return this->pipes[Pipe::WRITE];
    }

    auto operator=(const Pipe &other) -> Pipe & = delete;

    auto operator=(Pipe &&other) noexcept -> Pipe &
    {
        if (this != &other)
        {
            this->close_read();
            this->close_write();
            this->pipes[Pipe::READ] = other.pipes[Pipe::READ];
            this->pipes[Pipe::WRITE] = other.pipes[Pipe::WRITE];
            other.pipes[Pipe::READ] = Pipe::INVALID_DESCRIPTOR;
            other.pipes[Pipe::WRITE] = Pipe::INVALID_DESCRIPTOR;
        }

        return *this;
    }

private:
    static constexpr size_t READ = 0;
    static constexpr size_t WRITE = 1;
    static constexpr int INVALID_DESCRIPTOR = -1;
    int pipes[2] = {Pipe::INVALID_DESCRIPTOR, Pipe::INVALID_DESCRIPTOR};
};
#endif
