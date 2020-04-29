#include "File.hpp"

namespace abuild
{
File::File(std::filesystem::path path) :
    mPath{std::move(path)},
    mType{detectType(mPath.extension())}
{
}

auto File::path() const noexcept -> const std::filesystem::path &
{
    return mPath;
}

auto File::type() const noexcept -> Type
{
    return mType;
}

auto File::detectType(const std::filesystem::path &extension) noexcept -> Type
{
    if (extension == ".cpp" || extension == ".cxx" || extension == ".cc" || extension == ".c")
    {
        return Type::Source;
    }

    if (extension == ".hpp" || extension == ".hxx" || extension == ".h")
    {
        return Type::Header;
    }

    return Type::Other;
}
}
