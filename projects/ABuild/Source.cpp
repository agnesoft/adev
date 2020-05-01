#include "Source.hpp"

#include <functional>

namespace abuild
{
Source::Source(const std::filesystem::path &path) :
    File{path}
{
    scanContent();
}

auto Source::update() -> bool
{
    if (isChanged())
    {
        return scanContent();
    }

    return false;
}

auto Source::isChanged() const -> bool
{
    return mLastWriteTime != lastWriteTime();
}

auto Source::lastWriteTime() const -> std::filesystem::file_time_type
{
    return std::filesystem::last_write_time(path());
}

auto Source::scanContent() -> bool
{
    const std::string fileContent = content();
    const std::size_t hash = std::hash<std::string>{}(fileContent);

    if (hash != mHash)
    {
        mHash = hash;
        mLastWriteTime = lastWriteTime();
        return true;
    }

    return false;
}
}
