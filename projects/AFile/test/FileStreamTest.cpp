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

#include <AFile.hpp>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace filestreamtest
{
static constexpr const char *TEST_FILE = "afile.filestream.testfile";

class TestFile
{
public:
    TestFile()
    {
        removeFiles();
    }

    TestFile(const char *filename) :
        mFilename(filename)
    {
        removeFiles();
    }

    explicit TestFile(const char *content, size_t size)
    {
        std::fstream{TEST_FILE, std::ios::trunc | std::ios::in | std::ios::out}.write(content, size);
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
                INFO("Unable to remove file in filestreamtest::TestFile::~TestFile()");
            }
        }
        catch (...)
        {
            std::terminate();
        }
    }

    [[nodiscard]] const char *filename() const noexcept
    {
        return mFilename.c_str();
    }

    [[nodiscard]] std::vector<char> fileContent() const
    {
        std::vector<char> actual(std::filesystem::file_size(mFilename));
        std::fstream{mFilename}.read(actual.data(), actual.size());
        return actual;
    }

    TestFile &operator=(const TestFile &other) = delete;
    TestFile &operator=(TestFile &&other) = delete;

private:
    void removeFiles()
    {
        if (std::filesystem::exists(mFilename))
        {
            std::filesystem::permissions(mFilename, std::filesystem::perms::none | std::filesystem::perms::all);
        }

        std::filesystem::remove(mFilename);
    }

    const std::string mFilename = TEST_FILE;
};

TEST_CASE("FileStream(const char *filename) [afile::FileStream]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::FileStream stream{testFile.filename()};
        std::vector<char> actual(std::filesystem::file_size(testFile.filename()));
        afile::FileStream{testFile.filename()}.read(actual.data(), actual.size());
        REQUIRE(actual == std::vector<char>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[data]")
    {
        const std::vector<char> content{'0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '1', '0'};
        TestFile file{content.data(), static_cast<size_t>(content.size())};
        const afile::FileStream stream{file.filename()};
        REQUIRE(file.fileContent() == content);
    }
}

TEST_CASE("FileStream(FileStream &&other) noexcept [afile::FileStream]")
{
    const std::vector<char> content{'0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '1', '0'};
    TestFile file{content.data(), static_cast<size_t>(content.size())};
    afile::FileStream stream{file.filename()};
    afile::FileStream other{std::move(stream)};
    std::vector<char> data(content.size());
    other.read(data.data(), content.size());
    REQUIRE(data == content);
}

TEST_CASE("buffer() noexcept -> Buffer & [acore::DataStream]")
{
    TestFile testFile;
    afile::FileStream stream{testFile.filename()};
    REQUIRE(noexcept(stream.buffer()));
    REQUIRE(stream.buffer().filename() == std::string{testFile.filename()}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("buffer() const noexcept -> Buffer & [acore::DataStream]")
{
    TestFile testFile;
    const afile::FileStream stream{testFile.filename()};
    REQUIRE(noexcept(stream.buffer()));
    REQUIRE(stream.buffer().filename() == std::string{testFile.filename()}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("pos() const noexcept -> size_type [afile::FileStream]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::FileStream stream{TEST_FILE};
        REQUIRE(noexcept(stream.pos()));
        REQUIRE(stream.pos() == 0);
    }

    SECTION("[data]")
    {
        TestFile testFile;
        afile::FileStream stream{TEST_FILE};
        stream.write("Hello world!", 12);
        REQUIRE(std::as_const(stream).pos() == 12);
    }
}

TEST_CASE("read(char *data, size_type count) -> void [afile::FileStream]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        afile::FileStream stream{TEST_FILE};
        std::array<char, 10> buf{};
        REQUIRE_THROWS_AS(stream.read(buf.data(), 5), acore::Exception);
    }

    SECTION("[in bounds]")
    {
        TestFile testFile{"AbcdefgH", 8};
        afile::FileStream stream{TEST_FILE};
        std::vector<char> buf(3);
        stream.seek(2);
        stream.read(buf.data(), 3);
        REQUIRE(buf == std::vector<char>{'c', 'd', 'e'});
    }

    SECTION("[out of bounds]")
    {
        TestFile testFile{"123", 3};
        afile::FileStream stream{TEST_FILE};
        std::vector<char> buf(5);
        REQUIRE_THROWS_AS(stream.read(buf.data(), 5), acore::Exception);
    }
}

TEST_CASE("reset() -> void [afile::FileStream]")
{
    TestFile testFile;
    afile::FileStream stream{TEST_FILE};
    stream.write("hello world", 11);
    stream.reset();
    REQUIRE(stream.pos() == 0);
}

TEST_CASE("seek(size_type) -> void [afile::FileStream]")
{
    TestFile testFile;
    afile::FileStream stream{TEST_FILE};
    stream.seek(11);
    REQUIRE(stream.pos() == 11);
}

TEST_CASE("write(const char *data, size_type count) -> void [afile::FileStream]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        afile::FileStream{TEST_FILE}.write("Hello World!", 12);
        REQUIRE(testFile.fileContent() == std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'});
    }

    SECTION("[data]")
    {
        TestFile testFile{"Hello World!", 12};

        SECTION("[in bounds]")
        {
            {
                afile::FileStream stream{TEST_FILE};
                stream.seek(6);
                stream.write("There", 5);
            }
            REQUIRE(testFile.fileContent() == std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'T', 'h', 'e', 'r', 'e', '!'});
        }

        SECTION("[out of bounds]")
        {
            {
                afile::FileStream stream{TEST_FILE};
                stream.seek(11);
                stream.write(" Test!", 6);
            }
            REQUIRE(testFile.fileContent() == std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', ' ', 'T', 'e', 's', 't', '!'});
        }

        SECTION("[invalid pos]")
        {
            afile::FileStream stream{TEST_FILE};
            stream.seek(-1);
            REQUIRE_THROWS_AS(stream.write("Hello", 5), acore::Exception);
        }
    }
}

TEST_CASE("FileStream &operator=(FileStream &&other) noexcept [afile::FileStream]")
{
    const std::vector<char> content{'0', '0', '0', '0', '0', '0', '0', '4', '0', '0', '1', '0'};
    TestFile file{content.data(), static_cast<size_t>(content.size())};
    afile::FileStream stream{file.filename()};
    TestFile file2("afile.filestream.testfile2");
    afile::FileStream other{file2.filename()};
    other = std::move(stream);
    std::vector<char> data(content.size());
    other.read(data.data(), content.size());
    REQUIRE(data == content);
}

TEST_CASE("operator<<(FileStream &stream, const T &value) -> FileStream &, operator>>(FileStream &stream, T &value) -> FileStream & [afile::FileStream]")
{
    TestFile testFile;
    afile::FileStream stream{TEST_FILE};

    SECTION("bool")
    {
        stream << true << false;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(bool) + sizeof(bool)));
        stream.reset();
        bool t = false;
        bool f = true;
        stream >> t >> f;
        REQUIRE(t);
        REQUIRE(f == false);
    }

    SECTION("char")
    {
        stream << 'A' << 'B';
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(char) + sizeof(char)));
        stream.reset();
        char c = 0;
        char d = 0;
        stream >> c >> d;
        REQUIRE(c == 'A');
        REQUIRE(d == 'B');
    }

    SECTION("std::uint8_t, std::int8_t")
    {
        stream << std::numeric_limits<std::uint8_t>::max() << std::numeric_limits<std::int8_t>::min();
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(std::uint8_t) + sizeof(std::uint8_t)));
        stream.reset();
        std::uint8_t i = 0;
        std::int8_t j = 0;
        stream >> i >> j;
        REQUIRE(i == std::numeric_limits<std::uint8_t>::max());
        REQUIRE(j == std::numeric_limits<std::int8_t>::min());
    }

    SECTION("std::uint16_t, std::int16_t")
    {
        stream << std::numeric_limits<std::uint16_t>::max() << std::numeric_limits<std::int16_t>::min();
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(std::uint16_t) + sizeof(std::uint16_t)));
        stream.reset();
        std::uint16_t i = 0;
        std::int16_t j = 0;
        stream >> i >> j;
        REQUIRE(i == std::numeric_limits<std::uint16_t>::max());
        REQUIRE(j == std::numeric_limits<std::int16_t>::min());
    }

    SECTION("std::uint32_t, std::int32_t")
    {
        stream << std::numeric_limits<std::uint32_t>::max() << std::numeric_limits<std::int32_t>::min();
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(std::uint32_t) + sizeof(std::uint32_t)));
        stream.reset();
        std::uint32_t i = 0;
        std::int32_t j = 0;
        stream >> i >> j;
        REQUIRE(i == std::numeric_limits<std::uint32_t>::max());
        REQUIRE(j == std::numeric_limits<std::int32_t>::min());
    }

    SECTION("std::uint64_t, std::int64_t")
    {
        stream << std::numeric_limits<std::uint64_t>::max() << std::numeric_limits<std::int64_t>::min();
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(std::uint64_t) + sizeof(std::uint64_t)));
        stream.reset();
        std::uint64_t i = 0;
        std::int64_t j = 0;
        stream >> i >> j;
        REQUIRE(i == std::numeric_limits<std::uint64_t>::max());
        REQUIRE(j == std::numeric_limits<std::int64_t>::min());
    }

    SECTION("const char*, std::string")
    {
        const std::string str1{"Hello World!"};
        const std::string str2;
        const std::string str3{"Hello, World!"};

        stream << str1.data() << str2.data() << str3;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(acore::size_type) + str1.size() + sizeof(acore::size_type) + str2.size() + sizeof(acore::size_type) + str3.size()));

        std::string str5;
        std::string str6;
        std::string str7;

        stream.reset();
        stream >> str5 >> str6 >> str7;
        REQUIRE(str5 == str1);
        REQUIRE(str6 == str2);
        REQUIRE(str7 == str3);
    }

    SECTION("float")
    {
        const float i = -4.4F;
        const float j = 1.1023F;
        stream << i << j;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(float) + sizeof(float)));
        stream.reset();
        float k = 0.0F;
        float l = 0.0F;
        stream >> k >> l;
        REQUIRE(std::fabs(k - i) < std::numeric_limits<double>::epsilon());
        REQUIRE(std::fabs(l - j) < std::numeric_limits<double>::epsilon());
    }

    SECTION("double")
    {
        const double i = -10.102;
        const double j = 10.00000410007;
        stream << i << j;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(double) + sizeof(double)));
        stream.reset();
        double k = 0.0;
        double l = 0.0;
        stream >> k >> l;
        REQUIRE(std::fabs(k - i) < std::numeric_limits<double>::epsilon());
        REQUIRE(std::fabs(l - j) < std::numeric_limits<double>::epsilon());
    }

    SECTION("std::vector<int>")
    {
        std::vector<int> v1;
        std::vector<int> v2{1, 2, 3, 4};
        stream << v1 << v2;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(acore::size_type) + sizeof(acore::size_type) + (sizeof(int) * 4)));
        stream.reset();
        std::vector<int> v3;
        std::vector<int> v4;
        stream >> v3 >> v4;
        REQUIRE(v3 == v1);
        REQUIRE(v4 == v2);
    }

    SECTION("std::vector<std::string>")
    {
        const std::string str1{"Hello"};
        const std::string str2{"World"};
        const std::string str3{"!"};
        std::vector<std::string> v1{str1, str2, str3};
        std::vector<std::string> v2;
        stream << v1 << v2;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(acore::size_type) + sizeof(acore::size_type) + str1.size() + sizeof(acore::size_type) + str2.size() + sizeof(acore::size_type) + str3.size() + sizeof(acore::size_type)));
        stream.reset();
        std::vector<std::string> v3;
        std::vector<std::string> v4;
        stream >> v3 >> v4;
        REQUIRE(v3 == v1);
        REQUIRE(v4 == v2);
    }

    SECTION("std::pair<int, std::string>")
    {
        const std::string str1{"Hi"};
        std::pair pair1{4, str1};
        std::pair<int, std::string> pair2;
        stream << pair1 << pair2;
        REQUIRE(stream.buffer().size() == static_cast<acore::size_type>(sizeof(int) + sizeof(acore::size_type) + str1.size() + sizeof(int) + sizeof(acore::size_type)));
    }
}

TEST_CASE("operator<<(FileStream &&stream, const T &value) -> FileStream &&, operator>>(FileStream &&stream, T &value) -> FileStream &&")
{
    TestFile testFile;
    afile::FileStream{testFile.filename()} << std::vector<int>{1, 2, 3, 4, 5} << std::string{"Hello World"};
    const std::vector<char> data = testFile.fileContent();
    REQUIRE(data.size() == 47);
    std::string str;
    std::vector<int> vec;
    afile::FileStream{testFile.filename()} >> vec >> str;
    REQUIRE(str == std::string{"Hello World"});
    REQUIRE(vec == std::vector<int>{1, 2, 3, 4, 5});
}
}
