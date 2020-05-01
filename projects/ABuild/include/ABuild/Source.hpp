#ifndef ABUILD_SOURCE_HPP
#define ABUILD_SOURCE_HPP

#include "File.hpp"

namespace abuild
{
class Source : public File
{
public:
    explicit Source(const std::filesystem::path &path);

    [[nodiscard]] auto update() -> bool;

private:
    [[nodiscard]] auto lastWriteTime() const -> std::filesystem::file_time_type;
    auto scanContent() -> bool;

    std::filesystem::file_time_type mLastWriteTime;
    std::size_t mHash = 0;
};
}

#endif
