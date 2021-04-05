export module atest : source_location;

namespace atest
{
export template<typename T = int>
class source_location
{
public:
    [[nodiscard]] static auto current(const char *file = __builtin_FILE(), int line = __builtin_LINE()) noexcept -> source_location
    {
        source_location sourceLocation;
        sourceLocation.mFile = file;
        sourceLocation.mLine = line;
        return sourceLocation;
    }

    [[nodiscard]] constexpr auto file_name() const noexcept -> const char *
    {
        return mFile;
    }

    [[nodiscard]] constexpr auto line() const noexcept -> int
    {
        return mLine;
    }

private:
    const char *mFile = "unknown";
    int mLine = -1;
};
}
