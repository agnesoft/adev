#ifdef _MSC_VER
export module atest : source_location;
#endif

namespace atest
{
//! The `source_location<T>` is a placeholder class for C++20 std::source_location.
export template<typename T = int>
class source_location
{
public:
    //! Returns the source_location object containing the `file`
    //! filename and line number `line` of the call site. Since
    //! there is no support for `consteval` yet the template trick
    //! is used to force the compiler to evaluate it at the place
    //! of instantiation.
    [[nodiscard]] static auto current(const char *file = __builtin_FILE(), int line = __builtin_LINE()) noexcept -> source_location
    {
        source_location sourceLocation;
        sourceLocation.mFile = file;
        sourceLocation.mLine = line;
        return sourceLocation;
    }

    //! Returns the source file name. `unknown` if no file name was set.
    [[nodiscard]] constexpr auto file_name() const noexcept -> const char *
    {
        return mFile;
    }

    //! Returns the source file's line. `-1` if no line was set.
    [[nodiscard]] constexpr auto line() const noexcept -> int
    {
        return mLine;
    }

private:
    const char *mFile = "unknown";
    int mLine = -1;
};
}
