#include "File.hpp"

#include <fstream>

namespace abuild
{
File::File(std::filesystem::path path) :
    mPath{std::move(path)},
    mType{detectType(mPath.extension())},
    mLastWrite{lastWriteTime(mPath)},
    mContent{fileContent(mPath)}
{
}

auto File::isChanged() const -> bool
{
    return mLastWrite != lastWriteTime(mPath) && mContent != fileContent(mPath);
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

auto File::lastWriteTime(const std::filesystem::path &path) -> std::filesystem::file_time_type
{
    if (std::filesystem::exists(path))
    {
        return std::filesystem::last_write_time(path);
    }

    return {};
}

auto File::fileContent(const std::filesystem::path &path) -> std::string
{
    std::ifstream stream{path.string()};
    return std::string{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
}
}
