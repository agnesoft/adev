#include "File.hpp"

namespace abuild
{
File::File(std::filesystem::path path) :
    mPath(std::move(path))
{
}

auto File::path() const noexcept -> const std::filesystem::path &
{
    return mPath;
}
}
