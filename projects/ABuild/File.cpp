#include "File.hpp"

namespace abuild
{
File::File(std::filesystem::path path) :
    mPath(std::move(path))
{
}

const std::filesystem::path &File::path() const noexcept
{
    return mPath;
}
}
