#ifndef __clang__
export module awinapi : handle;
export import "windows.hpp";
export import astl;
#endif

#ifdef _WIN32
namespace awinapi
{
//! The `Handle` is a wrapper around WinAPI's
//! `::HANDLE` that closes it automatically on
//! destruction. You can access the mutable
//! ::HANDLE itself with `get()` method.
export class Handle
{
public:
    //! Default constructor;
    Handle() noexcept = default;

    //! Constructs the wrapper with `handle`.
    explicit Handle(::HANDLE handle) noexcept :
        handle{handle}
    {
    }

    //! Deleted copy constructor.
    Handle(const Handle &other) = delete;

    //! Move constructor.
    Handle(Handle &&other) noexcept
    {
        *this = std::move(other);
    }

    //! Destructor that closes the handle.
    ~Handle()
    {
        this->close();
    }

    //! Closes the handle on demand.
    auto close() -> void
    {
        if (this->handle != nullptr)
        {
            ::CloseHandle(this->handle);
            this->handle = nullptr;
        }
    }

    //! Returns the underlying `::HANDLE`.
    [[nodiscard]] constexpr auto get() noexcept -> ::HANDLE &
    {
        return this->handle;
    }

    //! Deleted copy assignment operator.
    auto operator=(const Handle &other) -> Handle & = delete;

    //! Move assignment operator.
    auto operator=(Handle &&other) noexcept -> Handle &
    {
        if (this != &other)
        {
            this->handle = other.handle;
            other.handle = nullptr;
        }

        return *this;
    }

private:
    ::HANDLE handle = nullptr;
};
}
#endif
