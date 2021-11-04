#ifndef ASTL_SOURCE_LOCATION_HPP
#define ASTL_SOURCE_LOCATION_HPP

namespace astl
{
export template<typename T>
class SourceLocationImpl
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
        return this->file;
    }

    [[nodiscard]] constexpr auto line() const noexcept -> std::uint_least32_t
    {
        return this->fileLine;
    }

private:
    const char *file = "";
    std::uint_least32_t fileLine = 0;
};
}

namespace std // NOLINT(cert-dcl58-cpp)
{
export class source_location : public astl::SourceLocationImpl<source_location> // NOLINT(readability-identifier-naming)
{
};
}

#endif
