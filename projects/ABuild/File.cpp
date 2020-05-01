#include "File.hpp"

#include <fstream>
#include <functional>

namespace abuild
{
File::File(std::filesystem::path path) :
    mPath{std::move(path)}
{
}

auto File::content() const -> std::string
{
    std::ifstream stream{path().string()};
    return std::string{std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
}

auto File::path() const noexcept -> const std::filesystem::path &
{
    return mPath;
}

auto File::type(const std::filesystem::path &path) -> Type
{
    if (path.extension().string() == ".cpp" || path.extension().string() == ".cxx" || path.extension().string() == ".cc" || path.extension().string() == ".c")
    {
        return Type::TranslationUnit;
    }

    if (path.extension().string() == ".hpp" || path.extension().string() == ".hxx" || path.extension().string() == ".h")
    {
        return Type::Header;
    }

    return Type::Other;
}
}
