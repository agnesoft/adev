#ifndef ASTL_SOURCE_LOCATION_HPP
#define ASTL_SOURCE_LOCATION_HPP

namespace astl
{
export template<typename T>
class source_location_impl
{
public:
    [[nodiscard]] static auto current(const char *file = __builtin_FILE(), int line = __builtin_LINE()) noexcept -> T
    {
        T sourceLocation;
        sourceLocation.file = file;
        sourceLocation.fileLine = line;
        return sourceLocation;
    }

    [[nodiscard]] constexpr auto file_name() const noexcept -> const char *
    {
        return file;
    }

    [[nodiscard]] constexpr auto line() const noexcept -> int
    {
        return fileLine;
    }

private:
    const char *file = "unknown";
    int fileLine = -1;
};
}

namespace std //NOLINT(cert-dcl58-cpp)
{
export class source_location : public astl::source_location_impl<source_location>
{
};
}

#endif
