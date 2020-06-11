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

#ifndef ABUILD_TEST_TEST_FILE_HPP
#define ABUILD_TEST_TEST_FILE_HPP

#include <filesystem>
#include <fstream>
#include <string>

namespace abuildtest
{
[[nodiscard]] auto testFileOpenMode() -> std::fstream::openmode;

class TestFile
{
public:
    explicit TestFile(const std::string &name);
    TestFile(const std::string &name, const std::string &content);
    TestFile(const TestFile &other) = delete;
    TestFile(TestFile &&other) = default;
    ~TestFile();

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &;

    auto operator=(const TestFile &other) -> TestFile & = delete;
    auto operator=(TestFile &&other) -> TestFile & = default;

private:
    void removeFile() const;

    std::filesystem::path mFile;
};
}

#endif
