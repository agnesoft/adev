#include "Source.hpp"

#include <functional>

namespace abuild
{
Source::Source(const std::filesystem::path &path) :
    File{path}
{
    static_cast<void>(update());
}

auto Source::update() -> bool
{
    if (mLastWriteTime != lastWriteTime())
    {
        mLastWriteTime = lastWriteTime();
        return scanContent();
    }

    return false;
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
        return true;
    }

    return false;
}
}
