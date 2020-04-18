#ifndef ABUILD_FILE_HPP
#define ABUILD_FILE_HPP

#include <filesystem>

namespace abuild
{
class File
{
public:
    explicit File(std::filesystem::path path);

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &;

private:
    std::filesystem::path mPath;
};
}

#endif
