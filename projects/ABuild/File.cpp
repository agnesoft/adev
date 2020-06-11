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

#include "File.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

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
