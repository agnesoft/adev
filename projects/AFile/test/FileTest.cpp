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

#include "pch.hpp"

#include <catch2/catch.hpp>

#include <filesystem>
#include <sstream>

namespace filetest
{
static constexpr const char *TEST_FILE = "afile.file.testfile";

class TestFile
{
public:
    TestFile()
    {
        removeFiles();
    }

    explicit TestFile(const char *filename) :
        mFilename(filename),
        mWALFilename('.' + mFilename)
    {
        removeFiles();
    }

    TestFile(const TestFile &other) = delete;
    TestFile(TestFile &&other) = delete;

    ~TestFile() noexcept
    {
        try
        {
            try
            {
                removeFiles();
            }
            catch (...)
            {
                INFO("Unable to remove files in filetest::TestFile::~TestFile()");
            }
        }
        catch (...)
        {
            std::terminate();
        }
    }

    [[nodiscard]] auto filename() const noexcept -> const char *
    {
        return mFilename.c_str();
    }

    [[nodiscard]] auto fileContent() const -> std::vector<char>
    {
        std::vector<char> actual(std::filesystem::file_size(mFilename));
        afile::FileStream{mFilename.c_str()}.read(actual.data(), actual.size());
        return actual;
    }

    [[nodiscard]] auto walFilename() const noexcept -> const char *
    {
        return mWALFilename.c_str();
    }

    auto operator=(const TestFile &other) -> TestFile & = delete;
    auto operator=(TestFile &&other) -> TestFile & = delete;

private:
    auto removeFiles() -> void
    {
        if (std::filesystem::exists(mFilename))
        {
            std::filesystem::permissions(mFilename, std::filesystem::perms::none | std::filesystem::perms::all);
        }

        std::filesystem::remove(mFilename);

        if (std::filesystem::exists(mWALFilename))
        {
            std::filesystem::permissions(mWALFilename, std::filesystem::perms::none | std::filesystem::perms::all);
        }

        std::filesystem::remove(mWALFilename);
    }

    const std::string mFilename = TEST_FILE;
    const std::string mWALFilename = "." + std::string{mFilename};
};

struct SerializationThrower
{
};
}

namespace acore
{
template<typename Buffer>
[[noreturn]] auto operator<<([[maybe_unused]] DataStreamBase<Buffer> &stream, [[maybe_unused]] const filetest::SerializationThrower &value) -> DataStreamBase<Buffer> &
{
    throw Exception{} << "filetest::SerializationThrower, operator<<"; //NOLINT(bugprone-unused-raii)
}
}

namespace filetest
{
TEST_CASE("File(const char *filename) [afile::File]")
{
    const TestFile testFile;

    SECTION("[new]")
    {
        {
            afile::File file{testFile.filename()};
        }
        REQUIRE(std::filesystem::exists(testFile.filename()));
    }

    SECTION("[valid]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(std::filesystem::exists(testFile.filename()));
        REQUIRE_NOTHROW(afile::File{testFile.filename()});
    }

    SECTION("[invalid]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{-1};
        REQUIRE(std::filesystem::exists(testFile.filename()));
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}, acore::Exception);
    }

    SECTION("[in use]")
    {
        afile::File file{testFile.filename()};
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}, acore::Exception);
    }

    SECTION("[write ahead log]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{2}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::string{"Hello Wo"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::FileStream{testFile.walFilename()} << acore::size_type{3}
                                                  << acore::size_type{0} << acore::size_type{8} << acore::size_type{1} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                  << acore::size_type{-52} << acore::size_type{0} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                  << acore::size_type{-68} << acore::size_type{0}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(std::filesystem::exists(testFile.filename()));
        REQUIRE(std::filesystem::exists(testFile.walFilename()));
        REQUIRE_NOTHROW(afile::File{testFile.filename()});
    }

    SECTION("[invalid write ahead log]")
    {
        afile::FileStream{testFile.walFilename()} << acore::size_type{0};
        std::filesystem::permissions(testFile.walFilename(), std::filesystem::perms::none | std::filesystem::perms::owner_read);
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}, acore::Exception);
    }
}

TEST_CASE("File(File &&other) noexcept [afile::File]")
{
    const TestFile testFile;
    afile::FileStream{testFile.filename()} << acore::size_type{3}
                                           << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    afile::File file{testFile.filename()};
    afile::File other{std::move(file)};
    REQUIRE(other.filename() == std::string{testFile.filename()});
    REQUIRE(other.size() == static_cast<acore::size_type>(std::filesystem::file_size(testFile.filename())));
}

TEST_CASE("beginWAL() -> void [afile::File]")
{
    const TestFile testFile;

    SECTION("[manual]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{2}
                                               << acore::size_type{0} << acore::size_type{11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        {
            afile::File file{testFile.filename()};
            file.beginWAL();
            file.remove(0);
            file.insert(std::vector<int>{1, 2, 3});
            file.endWAL();
        }

        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{2}
                                                                << acore::INVALID_INDEX << acore::size_type{-11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{0} << acore::size_type{20} << std::vector<int>{1, 2, 3}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[nested]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{2}
                                               << acore::size_type{0} << acore::size_type{11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        {
            afile::File file{testFile.filename()};
            file.beginWAL();
            file.remove(0);

            {
                file.beginWAL();
                file.insert(std::vector<int>{1, 2, 3});
                file.endWAL();
            }

            file.endWAL();
        }

        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{2}
                                                                << acore::INVALID_INDEX << acore::size_type{-11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{0} << acore::size_type{20} << std::vector<int>{1, 2, 3}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[nested unended]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{2}
                                               << acore::size_type{0} << acore::size_type{11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        {
            afile::File file{testFile.filename()};
            file.beginWAL();
            file.remove(0);

            {
                file.beginWAL();
                file.insert(std::vector<int>{1, 2, 3});
                file.endWAL();
            }
        }

        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{2}
                                                                << acore::size_type{0} << acore::size_type{11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[unended value update]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{2}
                                               << acore::size_type{0} << acore::size_type{11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        {
            afile::File file{testFile.filename()};
            file.beginWAL();
            file.insert(0, static_cast<acore::size_type>(sizeof(size_t)) + 1, 'B');
        }

        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{2}
                                                                << acore::size_type{0} << acore::size_type{11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[unended resize]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{1}
                                               << acore::size_type{0} << acore::size_type{11} << std::string{"abc"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        {
            afile::File file{testFile.filename()};
            file.beginWAL();
            file.resize(0, 19); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{1}
                                                                << acore::size_type{0} << acore::size_type{19} << std::string{"abc"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[unended copy]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{2}
                                               << acore::size_type{0} << acore::size_type{11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        {
            afile::File file{testFile.filename()};
            file.beginWAL();
            file.copy(1, 8, 14, 5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{2}
                                                                << acore::size_type{0} << acore::size_type{11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[unended copy beyond size]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{2}
                                               << acore::size_type{0} << acore::size_type{20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        {
            afile::File file{testFile.filename()};
            file.beginWAL();
            file.copy(0, 12, 24, 4); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{2}
                                                                << acore::size_type{0} << acore::size_type{20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[exception]")
    {
        const std::vector<char> fileContent = (acore::DataStream{} << acore::size_type{2}
                                                                   << acore::size_type{0} << acore::size_type{11} << std::string{"abc"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                   << acore::size_type{1} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                  .buffer()
                                                  .data();
        afile::FileStream{testFile.filename()}.write(fileContent.data(), fileContent.size());

        try
        {
            afile::File file{testFile.filename()};
            file.beginWAL();
            file.remove(0);
            file.insert(SerializationThrower{});
        }
        catch ([[maybe_unused]] acore::Exception &e)
        {
            REQUIRE(testFile.fileContent() == fileContent);
        }
    }
}

TEST_CASE("clear() -> void [afile::File]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::File{testFile.filename()}.clear();
        REQUIRE(testFile.fileContent() == std::vector<char>(8, 0));
    }

    SECTION("[data]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::File{testFile.filename()}.clear();
        REQUIRE(testFile.fileContent() == std::vector<char>(8, 0));
    }
}

TEST_CASE("count() const noexcept -> size_type [afile::File]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::File file{testFile.filename()};
        REQUIRE(noexcept(file.count()));
        REQUIRE(file.count() == 0);
    }

    SECTION("[data]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(afile::File{testFile.filename()}.count() == 3);
    }

    SECTION("[removed]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(afile::File{testFile.filename()}.count() == 2);
    }
}

TEST_CASE("endWAL() -> void [afile::File]")
{
    const TestFile testFile;
    afile::File{testFile.filename()}.endWAL();
    REQUIRE(testFile.fileContent() == std::vector<char>(8, 0));
}

TEST_CASE("filename() const noexcept -> const char * [afile::File]")
{
    const TestFile testFile;
    const afile::File file{testFile.filename()};
    REQUIRE(noexcept(file.filename()));
    REQUIRE(file.filename() == std::string{testFile.filename()});
}

TEST_CASE("indexes() const -> std::vector<size_type> [afile::File]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        REQUIRE(afile::File{testFile.filename()}.indexes() == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[data]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(afile::File{testFile.filename()}.indexes() == std::vector<acore::size_type>{0, 2});
    }
}

TEST_CASE("insert(const T &value) -> size_type [afile::File]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        REQUIRE(afile::File{testFile.filename()}.insert(std::string{"Hello World!"}) == 0);
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{1}
                                                                << acore::size_type{0} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[data]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(afile::File{testFile.filename()}.insert(std::vector<char>{'d', 'e', 'f'}) == 3);
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{4}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{1} << acore::size_type{20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{3} << acore::size_type{11} << std::vector<char>{'d', 'e', 'f'}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[large value]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{1} << acore::size_type{20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(afile::File{testFile.filename()}.insert(std::vector<int>(10000, 1)) == 3);
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{4} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{1} << acore::size_type{20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{3} << acore::size_type{40008} << std::vector<int>(10000, 1)) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        const bool isEqual = testFile.fileContent() == expected;
        REQUIRE(isEqual);
    }

    SECTION("[removed]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(afile::File{testFile.filename()}.insert(std::vector<char>{'a', 'b', 'c'}) == 1);
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{1} << acore::size_type{11} << std::vector<char>{'a', 'b', 'c'}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }
}

TEST_CASE("insert(size_type index, size_type offset, const T &value) -> void [afile::File]")
{
    const TestFile testFile;
    afile::FileStream{testFile.filename()} << acore::size_type{3}
                                           << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    SECTION("[update value]")
    {
        afile::File{testFile.filename()}.insert(0, 16, 10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 10, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[value overrun]")
    {
        afile::File{testFile.filename()}.insert(0, 32, 10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::INVALID_INDEX << acore::size_type{-28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{0} << acore::size_type{36} << std::vector<int>{1, 2, 3, 4, 5} << 0 << 10) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[value overrun at end of file]")
    {
        afile::File{testFile.filename()}.insert(2, 13, std::string{"There"}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{26} << acore::size_type{12} << 'H' << 'e' << 'l' << 'l' << 'o' << std::string{"There"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[missing index]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.insert(3, 1, 10), acore::Exception);
    }

    SECTION("[negative offset]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.insert(0, -1, 10), acore::Exception);
    }
}

TEST_CASE("isEmpty() const noexcept -> bool [afile::File]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        REQUIRE(afile::File{testFile.filename()}.isEmpty());
    }

    SECTION("[data]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE_FALSE(afile::File{testFile.filename()}.isEmpty());
    }
}

TEST_CASE("loadValue(size_type index, T &value) const -> void [afile::File]")
{
    const TestFile testFile;
    afile::FileStream{testFile.filename()} << acore::size_type{3}
                                           << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    SECTION("[missing]")
    {
        std::string str;
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.loadValue(1, str), acore::Exception);
    }

    SECTION("[existing]")
    {
        std::string str;
        afile::File{testFile.filename()}.loadValue(2, str);
        REQUIRE(str == std::string{"Hello World!"});
    }

    SECTION("[partial]")
    {
        acore::size_type i = 0;
        afile::File{testFile.filename()}.loadValue(2, i);
        REQUIRE(i == 12);
    }

    SECTION("[different type]")
    {
        std::vector<char> value;
        afile::File{testFile.filename()}.loadValue(2, value);
        REQUIRE(value == std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'});
    }

    SECTION("[wrong type]")
    {
        std::vector<int> value;
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.loadValue(2, value), acore::Exception);
    }
}

TEST_CASE("loadValue(size_type index, size_type offset, T &value) const -> void [afile::File]")
{
    const TestFile testFile;
    afile::FileStream{testFile.filename()} << acore::size_type{3}
                                           << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    int value = 0;

    SECTION("[missing index]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.loadValue(10, 8, value), acore::Exception);
    }

    SECTION("[valid]")
    {
        afile::File{testFile.filename()}.loadValue(0, 8, value); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(value == 1);
    }

    SECTION("[negative offset]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.loadValue(0, -1, value), acore::Exception);
    }

    SECTION("[offset beyond value size]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.loadValue(0, 100, value), acore::Exception);
    }

    SECTION("[size overrun]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.loadValue(0, 26, value), acore::Exception);
    }
}

TEST_CASE("copy(size_type index, size_type offset, size_type newOffset, size_type size) -> void [afile::File]")
{
    const TestFile testFile;
    afile::FileStream{testFile.filename()} << acore::size_type{3}
                                           << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    SECTION("[missing index]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.copy(10, 8, 16, 8), acore::Exception);
    }

    SECTION("[copy left]")
    {
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{3, 4, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        afile::File{testFile.filename()}.copy(0, 16, 8, 8); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[copy right]")
    {
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 1, 2, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        afile::File{testFile.filename()}.copy(0, 8, 16, 8); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[copy outside of value size]")
    {
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::INVALID_INDEX << acore::size_type{-28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{0} << acore::size_type{40} << std::vector<int>{1, 2, 3, 4, 5} << 0 << 1 << 2) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        afile::File{testFile.filename()}.copy(0, 8, 32, 8); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[to itself]")
    {
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        afile::File{testFile.filename()}.copy(0, 12, 12, 8); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[negative offset]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.copy(0, -8, 16, 8), acore::Exception);
    }

    SECTION("[negative new offset]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.copy(0, 8, -16, 8), acore::Exception);
    }

    SECTION("[negative size]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.copy(0, 8, 16, -8), acore::Exception);
    }

    SECTION("[size over value size]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.copy(0, 8, 16, 50), acore::Exception);
    }
}

TEST_CASE("optimize() -> void [afile::File]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::File{testFile.filename()}.optimize();
        REQUIRE(testFile.fileContent() == std::vector<char>(8, 0));
    }

    SECTION("[clean]")
    {
        const std::vector<char> data = (acore::DataStream{} << acore::size_type{3}
                                                            << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                            << acore::size_type{1} << acore::size_type{20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                            << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           .buffer()
                                           .data();
        afile::FileStream{testFile.filename()}.write(data.data(), data.size());
        afile::File{testFile.filename()}.optimize();
        REQUIRE(testFile.fileContent() == data);
    }

    SECTION("[dirty]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::File{testFile.filename()}.optimize();
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }
}

TEST_CASE("remove(size_type index) -> void [afile::File]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::File{testFile.filename()}.remove(0);
        REQUIRE(testFile.fileContent() == std::vector<char>(8, 0));
    }

    SECTION("[data]")
    {
        const std::vector<char> data = (acore::DataStream{} << acore::size_type{3}
                                                            << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                            << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                            << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           .buffer()
                                           .data();
        afile::FileStream{testFile.filename()}.write(data.data(), data.size());

        SECTION("[existing]")
        {
            afile::File{testFile.filename()}.remove(0);
            const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                    << acore::INVALID_INDEX << acore::size_type{-28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                    << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                    << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                   .buffer()
                                                   .data();
            REQUIRE(testFile.fileContent() == expected);
        }

        SECTION("[at end]")
        {
            afile::File{testFile.filename()}.remove(2);
            const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                    << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                    << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                   .buffer()
                                                   .data();
            REQUIRE(testFile.fileContent() == expected);
        }

        SECTION("[missing]")
        {
            afile::File{testFile.filename()}.remove(10); //NOLINT(readability-container-size-empty
            REQUIRE(testFile.fileContent() == data);
        }
    }
}

TEST_CASE("resize(size_type index, size_type size) -> void [afile::File]")
{
    const TestFile testFile;
    const std::vector<char> data = (acore::DataStream{} << acore::size_type{3}
                                                        << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                        << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                        << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                       .buffer()
                                       .data();
    afile::FileStream{testFile.filename()}.write(data.data(), data.size());

    SECTION("[same]")
    {
        afile::File{testFile.filename()}.resize(0, 28); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(testFile.fileContent() == data);
    }

    SECTION("[smaller]")
    {
        afile::File{testFile.filename()}.resize(0, 20); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::INVALID_INDEX << acore::size_type{-28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{0} << acore::size_type{20} << acore::size_type{5} << 1 << 2 << 3) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[smaller at end]")
    {
        afile::File{testFile.filename()}.resize(2, 13); //NOLINT(readability-container-size-empty
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5}
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3}
                                                                << acore::size_type{2} << acore::size_type{13} << acore::size_type{12} << 'H' << 'e' << 'l' << 'l' << 'o')
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[greater]")
    {
        afile::File{testFile.filename()}.resize(0, 36); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::INVALID_INDEX << acore::size_type{-28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{0} << acore::size_type{36} << std::vector<int>{1, 2, 3, 4, 5} << 0 << 0) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[greater at end]")
    {
        afile::File{testFile.filename()}.resize(2, 24); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const std::vector<char> expected = (acore::DataStream{} << acore::size_type{3}
                                                                << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                                                << acore::size_type{2} << acore::size_type{24} << std::string{"Hello World!"} << 0) //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               .buffer()
                                               .data();
        REQUIRE(testFile.fileContent() == expected);
    }

    SECTION("[missing]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.resize(10, 100), acore::Exception);
    }

    SECTION("[negative size]")
    {
        REQUIRE_THROWS_AS(afile::File{testFile.filename()}.resize(0, -1), acore::Exception);
    }
}

TEST_CASE("size() const noexcept -> size_type [afile::File]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        REQUIRE(afile::File{testFile.filename()}.size() == 8);
    }

    SECTION("[data]")
    {
        afile::FileStream{testFile.filename()} << acore::size_type{3}
                                               << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                               << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(afile::File{testFile.filename()}.size() == 124);
    }
}

TEST_CASE("size(size_type index) const -> size_type [afile::File]")
{
    const TestFile testFile;
    afile::FileStream{testFile.filename()} << acore::size_type{3}
                                           << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    SECTION("[existing]")
    {
        REQUIRE(afile::File{testFile.filename()}.size(0) == 28);
    }

    SECTION("[missing]")
    {
        REQUIRE(afile::File{testFile.filename()}.size(1) == 0);
    }

    SECTION("[invalid]")
    {
        REQUIRE(afile::File{testFile.filename()}.size(999) == 0);
    }
}

TEST_CASE("value(size_type index) const -> T [afile::File]")
{
    const TestFile testFile;
    afile::FileStream{testFile.filename()} << acore::size_type{3}
                                           << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    SECTION("[existing]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<std::vector<int>>(0) == std::vector<int>{1, 2, 3, 4, 5});
    }

    SECTION("[missing]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<std::vector<int>>(1) == std::vector<int>{}); //NOLINT(readability-container-size-empty
    }

    SECTION("[partial]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<acore::size_type>(2) == 12);
    }

    SECTION("[different type]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<std::vector<char>>(2) == std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'});
    }

    SECTION("[wrong type]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<std::vector<int>>(2) == std::vector<int>{}); //NOLINT(readability-container-size-empty
    }
}

TEST_CASE("value(size_type index, size_type offset) const -> T [afile::File]")
{
    const TestFile testFile;
    afile::FileStream{testFile.filename()} << acore::size_type{3}
                                           << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

    SECTION("[missing index]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<int>(10, 8) == 0);
    }

    SECTION("[valid]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<int>(0, 8) == 1);
    }

    SECTION("[negative offset]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<int>(0, -1) == 0);
    }

    SECTION("[offset beyond value size]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<int>(0, 100) == 0);
    }

    SECTION("[size overrun]")
    {
        REQUIRE(afile::File{testFile.filename()}.value<int>(0, 26) == 0);
    }
}

TEST_CASE("operator=(File &&other) noexcept -> File & [afile::File]")
{
    const TestFile testFile;
    const TestFile otherFile{"other.test.file"};
    afile::FileStream{testFile.filename()} << acore::size_type{3}
                                           << acore::size_type{0} << acore::size_type{28} << std::vector<int>{1, 2, 3, 4, 5} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::INVALID_INDEX << acore::size_type{-20} << std::vector<int>{1, 2, 3} //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           << acore::size_type{2} << acore::size_type{20} << std::string{"Hello World!"}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    afile::File file{testFile.filename()};
    afile::File other{otherFile.filename()};
    other = std::move(file);
    REQUIRE(other.filename() == std::string{testFile.filename()});
    REQUIRE(other.size() == static_cast<acore::size_type>(std::filesystem::file_size(testFile.filename())));
}

TEST_CASE("acore::File [examples]")
{
    TestFile testFile{"MyFile"};

    SECTION("[vector]")
    {
        // clang-format off
//! [[vector]]
afile::File file{"MyFile"};
const acore::size_type index = file.insert(std::vector<int>{1, 2, 3, 4});
const auto thirdValueOffset = static_cast<acore::size_type>(sizeof(size_t) + 2 * sizeof(int));
file.insert(index, thirdValueOffset, -1);
const auto vec = file.value<std::vector<int>>(index);
//{1, 2, 10, 4, 5}
//! [[vector]]
        // clang-format on

        REQUIRE(vec == std::vector<int>{1, 2, -1, 4});

        // clang-format off
//! [[extend_vector]]
const auto newValueOffset = static_cast<acore::size_type>(sizeof(size_t) + vec.size() * sizeof(int));
file.insert(index, newValueOffset, 3);
file.insert(index, 0, vec.size() + 1); //updates the vector's size
auto extendedVec = file.value<std::vector<int>>(index);
//{1, 2, 10, 4, 5, 50}
//! [[extend_vector]]
        // clang-format on

        REQUIRE(extendedVec == std::vector<int>{1, 2, -1, 4, 3});
    }
}
}
