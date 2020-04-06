#ifndef ABUILD_FILE_HPP
#define ABUILD_FILE_HPP

#include <filesystem>

namespace abuild
{
class File
{
public:
  explicit File(std::filesystem::path path);

      [[nodiscard]] const std::filesystem::path &path() const noexcept;

private:
    std::filesystem::path mPath;
};
}

#endif
