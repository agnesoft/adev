// Copyright 2020 Michael Vlach
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Source.hpp"

#include <functional>
#include <string_view>

namespace abuild
{
Source::Source(const std::filesystem::path &path) :
    File{path}
{
    static_cast<void>(update());
}

auto Source::includes() const noexcept -> const std::vector<std::string> &
{
    return mIncludes;
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

auto Source::extractInclude(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator
{
    it = skipWhiteSpace(it, end);
    std::optional<std::string::const_iterator> optional = skipInclude(it, end);

    if (optional)
    {
        it = saveInclude(skipWhiteSpace(*optional, end), end);
    }

    return it;
}

auto Source::extractIncludes(const std::string &sourceContent) -> void
{
    mIncludes.clear();

    std::string buffer;

    for (auto it = sourceContent.begin(), end = sourceContent.end(); it != end;)
    {
        if (*it == '#')
        {
            it = extractInclude(it, end);
        }
        else if (*it == '/')
        {
            it = skipComment(it, end);
        }
        else if (*it == '"')
        {
            it = skipStringLiteral(it, end);
        }
        else
        {
            ++it;
        }
    }
}

auto Source::lastWriteTime() const -> std::filesystem::file_time_type
{
    return std::filesystem::last_write_time(path());
}

auto Source::saveInclude(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator
{
    if (it != end && (*it == '"' || *it == '<'))
    {
        std::string include;

        while (++it != end && *it != '"' && *it != '>' && *it != '\n')
        {
            include.push_back(*it);
        }

        if (*it == '"' || *it == '>')
        {
            mIncludes.push_back(include);
        }
    }

    return it;
}

auto Source::scanContent() -> bool
{
    const std::string sourceContent = content();
    const std::size_t hash = std::hash<std::string>{}(sourceContent);

    if (hash != mHash)
    {
        mHash = hash;
        extractIncludes(sourceContent);
        return true;
    }

    return false;
}

auto Source::skipComment(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator
{
    if (++it != end)
    {
        if (*it == '/')
        {
            return skipLineComment(it, end);
        }

        if (*it == '*')
        {
            return skipMultiLineComment(it, end);
        }
    }

    return it;
}

auto Source::skipInclude(std::string::const_iterator it, std::string::const_iterator end) -> std::optional<std::string::const_iterator>
{
    for (char c : std::string_view("include"))
    {
        if (*it != c || ++it == end)
        {
            return {};
        }
    }

    return it;
}

auto Source::skipLineComment(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator
{
    while (++it != end && *it != '\n')
    {
    }

    return it;
}

auto Source::skipMultiLineComment(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator
{
    while (++it != end && !(*it == '*' && ++it != end && *it == '/'))
    {
    }

    return it;
}

auto Source::skipStringLiteral(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator
{
    bool escape = false;

    while (++it != end && (!escape && *it == '"'))
    {
        escape = (*it == '\\');
    }

    return it;
}

auto Source::skipWhiteSpace(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator
{
    while (++it != end && (*it == ' ' || *it == '\t'))
    {
    }

    return it;
}
}
