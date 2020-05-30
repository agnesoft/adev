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

#include "TestFile.hpp"

#include <catch2/catch.hpp>

namespace abuildtest
{

TestFile::TestFile(const std::string &name) :
    mFile{name}
{
    removeFile();
}

TestFile::TestFile(const std::string &name, const std::string &content) :
    mFile{name}
{
    std::fstream stream{mFile.string(), testFileOpenMode()};
    stream.write(content.data(), content.size());
}

TestFile::~TestFile()
{
    try
    {
        try
        {
            removeFile();
        }
        catch (...)
        {
            INFO("Unable to remove file '" + mFile.string() + "' in abuildtest::TestFile::~TestFile()");
        }
    }
    catch (...)
    {
        std::terminate();
    }
}

auto TestFile::path() const noexcept -> const std::filesystem::path &
{
    return mFile;
}

void TestFile::removeFile() const
{
    if (std::filesystem::exists(mFile))
    {
        std::filesystem::remove(mFile);
    }
}

auto testFileOpenMode() -> std::fstream::openmode
{
    return static_cast<std::fstream::openmode>(static_cast<unsigned int>(std::ios::in) | static_cast<unsigned int>(std::ios::out) | static_cast<unsigned int>(std::ios::trunc));
}
}
