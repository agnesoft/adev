#ifndef ABUILD_FILE
#define ABUILD_FILE

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
