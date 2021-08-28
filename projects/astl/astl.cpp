export module astl;

export import "astl.hpp";

#if !__has_include(<source_location>)
namespace astl
{
//! The `source_location_impl<T>` is a placeholder
//! implementation of C++20 std::source_location.
template<typename T>
class source_location_impl
{
public:
    //! Returns the source_location object containing the `file`
    //! filename and line number `line` of the call site. Since
    //! there is no support for `consteval` yet the template trick
    //! is used to force the compiler to evaluate it at the place
    //! of instantiation.
    [[nodiscard]] static auto current(const char *file = __builtin_FILE(), int line = __builtin_LINE()) noexcept -> T
    {
        T sourceLocation;
        sourceLocation.file = file;
        sourceLocation.fileLine = line;
        return sourceLocation;
    }

    //! Returns the source file name or `unknown` if
    //! no file name was set.
    [[nodiscard]] constexpr auto file_name() const noexcept -> const char *
    {
        return file;
    }

    //! Returns the source file's line or `-1` if
    //! no line was set.
    [[nodiscard]] constexpr auto line() const noexcept -> int
    {
        return fileLine;
    }

private:
    const char *file = "unknown";
    int fileLine = -1;
};
}

namespace std
{
export class source_location : public atest::source_location_impl<source_location>
{
};
}
#endif