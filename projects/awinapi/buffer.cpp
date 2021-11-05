#ifndef __clang__
export module awinapi : buffer;
export import "windows.hpp";
export import astl;
#endif

#ifdef _WIN32
namespace awinapi
{
//! The `Buffer` is a wrapper around WinAPI string
//! buffers.
export class Buffer
{
public:
    //! Default constructor with `nullptr`
    //! internal buffer.
    Buffer() noexcept = default;

    //! Constructs the object by allocating `size`
    //! bytes in a buffer all initialized to zero.
    explicit Buffer(DWORD size) :
        buffer{static_cast<LPTSTR>(::LocalAlloc(LMEM_ZEROINIT, size))},
        bufferSize{size / static_cast<DWORD>(sizeof(TCHAR))}
    {
    }

    //! Deleted copy constructor.
    Buffer(const Buffer &other) = delete;

    //! Mmove constructor.
    Buffer(Buffer &&other) noexcept
    {
        *this = std::move(other);
    }

    //! Destructor that frees the internal buffer.
    ~Buffer()
    {
        ::LocalFree(this->buffer);
    }

    //! Returns the buffer size in bytes, i.e.
    //! `tchar_size() * sizeof(TCHAR)`.
    [[nodiscard]] auto byte_size() const noexcept -> DWORD
    {
        return this->bufferSize * static_cast<DWORD>(sizeof(TCHAR));
    }

    //! Returns mutable internal buffer size in
    //! `TCHAR`.
    [[nodiscard]] auto tchar_size() noexcept -> DWORD &
    {
        return this->bufferSize;
    }

    //! Returns the internal buffer as `LPTSTR`
    //! (pointer to a string).
    [[nodiscard]] auto str() noexcept -> LPTSTR &
    {
        return this->buffer;
    }

    //! Deleted copy assignment operator.
    Buffer &operator=(const Buffer &other) = delete;

    //! Move assignment operator.
    Buffer &operator=(Buffer &&other) noexcept
    {
        if (this != &other)
        {
            this->buffer = other.buffer;
            this->bufferSize = other.bufferSize;
            other.buffer = nullptr;
            other.bufferSize = 0u;
        }

        return *this;
    }

private:
    LPTSTR buffer = nullptr;
    DWORD bufferSize = 0u;
};
}
#endif
