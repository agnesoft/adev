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
#include <type_traits>

namespace afile
{
template<typename T, typename Container>
[[nodiscard]] constexpr auto operator==(const PersistentVector<T> &left, const Container &right) -> bool
{
    return std::equal(left.cbegin(), left.cend(), right.begin(), right.end());
}

template<typename T, typename Container>
[[nodiscard]] constexpr auto operator!=(const PersistentVector<T> &left, const Container &right) -> bool
{
    return !(left == right);
}
}

namespace persistentvectortest
{
static constexpr const char *TEST_FILE = "afile.persistentvector.testfile";

class TestFile
{
public:
    TestFile()
    {
        removeFiles();
        mFile = std::make_unique<afile::File>(TEST_FILE);
    }

    TestFile(const TestFile &other) = delete;
    TestFile(TestFile &&other) = default;

    ~TestFile() noexcept
    {
        try
        {
            try
            {
                mFile.reset();
                removeFiles();
            }
            catch (...)
            {
                INFO("Unable to remove file in persistentvectortest::TestFile::~TestFile()");
            }
        }
        catch (...)
        {
            std::terminate();
        }
    }

    [[nodiscard]] auto file() const noexcept -> afile::File *
    {
        return mFile.get();
    }

    auto operator=(const TestFile &other) -> TestFile & = delete;
    auto operator=(TestFile &&other) -> TestFile & = default;

private:
    static auto removeFiles() -> void
    {
        if (std::filesystem::exists(TEST_FILE))
        {
            std::filesystem::remove(TEST_FILE);
        }

        if (std::filesystem::exists(std::string{"."} + TEST_FILE))
        {
            std::filesystem::remove(std::string{"."} + TEST_FILE);
        }
    }

    std::unique_ptr<afile::File> mFile;
};

TEST_CASE("[afile::PersistentVector]")
{
    REQUIRE_FALSE(std::is_copy_constructible_v<afile::PersistentVector<int>>);
    REQUIRE_FALSE(std::is_copy_assignable_v<afile::PersistentVector<int>>);
    REQUIRE(std::is_nothrow_move_constructible_v<afile::PersistentVector<int>>);
    REQUIRE(std::is_nothrow_move_assignable_v<afile::PersistentVector<int>>);
    REQUIRE(std::is_nothrow_destructible_v<afile::PersistentVector<int>>);
}

TEST_CASE("PersistentVector(File *file) [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<int> vector{testFile.file()};
    REQUIRE(vector == std::array<int, 0>{});
}

TEST_CASE("PersistentVector(File *file, size_type index) [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[valid]")
    {
        const acore::size_type index = afile::PersistentVector<char>{{'A', 'B', 'C', 'D', 'E'}, testFile.file()}.fileIndex();
        const afile::PersistentVector<char> vector{testFile.file(), index};
        REQUIRE(vector == std::array<char, 5>{'A', 'B', 'C', 'D', 'E'});
    }

    SECTION("[missing data]")
    {
        REQUIRE_THROWS_AS((afile::PersistentVector<char>{testFile.file(), 0}), acore::Exception);
        REQUIRE_THROWS_AS((afile::PersistentVector<int>{testFile.file(), 0}), acore::Exception);
    }

    SECTION("[invalid data]")
    {
        testFile.file()->insert(acore::size_type{-1});
        REQUIRE_THROWS_AS((afile::PersistentVector<char>{testFile.file(), 0}), acore::Exception);
        REQUIRE_THROWS_AS((afile::PersistentVector<int>{testFile.file(), 0}), acore::Exception);
    }
}

TEST_CASE("PersistentVector(size_type size, File *file) [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<char> vector(5, testFile.file());
    REQUIRE(vector == std::array<char, 5>{char{}, char{}, char{}, char{}, char{}});
}

TEST_CASE("PersistentVector(size_type, const T &value, File *file) [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<float> vector(4, 1.1F, testFile.file()); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(vector == std::array<float, 4>{1.1F, 1.1F, 1.1F, 1.1F}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
}

TEST_CASE("PersistenVector(It first, It last, File *file) [afile::PersistentVector]")
{
    const TestFile testFile;
    std::array<double, 6> container{-1.0, -3.0, -4.5, 3.14, 6.67, -111.9}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    const afile::PersistentVector<double> vector{container.begin(), container.end(), testFile.file()};
    REQUIRE(vector == container);
}

TEST_CASE("PersistentVector(std::initializer_list<T> list, File *file) [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<double> vector{{-1.0, -3.0, -4.5, 3.14, 6.67, -111.9}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(vector == std::array<double, 6>{-1.0, -3.0, -4.5, 3.14, 6.67, -111.9}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
}

TEST_CASE("PersistentVector(Vector &&other [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<int> vector{testFile.file()};
        const afile::PersistentVector<int> other{std::move(vector)};
        REQUIRE(other == std::array<int, 0>{});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'A', 'B', 'C', 'D', 'E'}, testFile.file()};
        const afile::PersistentVector<char> other{std::move(vector)};
        REQUIRE(other == std::array<char, 5>{'A', 'B', 'C', 'D', 'E'});
    }
}

TEST_CASE("assign(IteratorType first, IteratorType last) -> void [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<float> vector{testFile.file()};
        const std::array<float, 5> range{1.0F, 2.0F, 3.0F, 4.0F, 5.0F}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        vector.assign(range.begin(), range.end());
        REQUIRE(vector == range);
    }

    SECTION("[data]")
    {
        afile::PersistentVector<float> vector{{4.4F, 2.1F, -1.121212F}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const std::array<float, 5> range{1.0F, 2.0F, 3.0F, 4.0F, 5.0F}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        vector.assign(range.begin(), range.end());
        REQUIRE(vector == range);
    }
}

TEST_CASE("assign(std::initializer_list<T> list) -> void [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        const std::initializer_list<char> list{'A', 'B', 'C'};
        vector.assign(list);
        REQUIRE(vector == list);
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'H', 'e', 'l', 'l', 'o', '!'}, testFile.file()};
        const std::initializer_list<char> list{'\0', '\n', '0', '1', ' '};
        vector.assign(list);
        REQUIRE(vector == list);
    }
}

TEST_CASE("at(size_type index) -> reference [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[data]")
    {
        afile::PersistentVector<int> vector{{-25, -20, -30}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        SECTION("[in bounds]")
        {
            vector.at(1) = 10; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(vector == std::array<int, 3>{-25, 10, -30});
        }

        SECTION("[out of bounds]")
        {
            REQUIRE_THROWS_AS(vector.at(-1), acore::Exception);
        }
    }

    SECTION("[empty]")
    {
        afile::PersistentVector<acore::size_type> vector{testFile.file()};
        REQUIRE_THROWS_AS(vector.at(0), acore::Exception);
    }
}

TEST_CASE("at(size_type index) const -> const_reference [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[data]")
    {
        const afile::PersistentVector<char> vector{{'H', 'W', '!', '\n'}, testFile.file()};

        SECTION("[in bounds]")
        {
            REQUIRE(*vector.at(2) == '!');
        }

        SECTION("[out of bounds]")
        {
            REQUIRE_THROWS_AS(vector.at(5), acore::Exception);
        }
    }

    SECTION("[empty]")
    {
        const afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE_THROWS_AS(vector.at(0), acore::Exception);
    }
}

TEST_CASE("back() -> reference [afile::PersistentVector]")
{
    const TestFile testFile;
    afile::PersistentVector<float> vector{{0.0F, 1.1F, 2.2F, 3.3F}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    vector.back() = 4.4F; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(vector == std::array<float, 4>{0.0F, 1.1F, 2.2F, 4.4F}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
}

TEST_CASE("back() const -> const_reference [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<double> vector{{10.0, -11.0, 12.2, -13.3}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(vector.back() == -13.3); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
}

TEST_CASE("begin() noexcept -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(noexcept(vector.begin()));
        REQUIRE(vector.begin() == vector.end());
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'G', '9', '8', 'F', 'x'}, testFile.file()};
        *vector.begin() = 'A';
        REQUIRE(vector == std::array<char, 5>{'A', '9', '8', 'F', 'x'});
    }
}

TEST_CASE("begin() const noexcept -> const_iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(noexcept(vector.begin()));
        REQUIRE(vector.begin() == vector.end());
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<int> vector{{-2, 2}, testFile.file()};
        REQUIRE(*vector.begin() == -2);
    }
}

TEST_CASE("capacity() const noexcept -> size_type [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<int> vector{testFile.file()};
        REQUIRE(noexcept(vector.capacity()));
        REQUIRE(vector.capacity() == 0);
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'m', 'c', 't', 's', '!'}, testFile.file()};

        SECTION("[constructed]")
        {
            REQUIRE(std::as_const(vector).capacity() == 5);
        }

        SECTION("[grow]")
        {
            vector.push_back('!');
            REQUIRE(std::as_const(vector).capacity() == 8);
        }

        SECTION("[shrink]")
        {
            vector.pop_back();
            REQUIRE(std::as_const(vector).capacity() == 5);
        }

        SECTION("[clear]")
        {
            vector.clear();
            REQUIRE(std::as_const(vector).capacity() == 5);
        }
    }
}

TEST_CASE("cbegin() const noexcept -> const_iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(noexcept(vector.cbegin()));
        REQUIRE(vector.cbegin() == vector.cend());
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<double> vector{{-14.0, -3.14}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(*vector.cbegin() == -14.0); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
}

TEST_CASE("cend() const noexcept -> const_iterator [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<int> vector{{-99, 100, 1000}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(noexcept(vector.cend()));
    REQUIRE(vector.cbegin() != vector.cend());
}

TEST_CASE("clear() noexcept -> void [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(noexcept(vector.clear()));
        vector.clear();
        REQUIRE(vector == std::array<char, 0>{});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<float> vector{{1.0F, -3.14F, 0.0F}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        vector.clear();
        REQUIRE(vector == std::array<float, 0>{});
    }
}

TEST_CASE("crbegin() const noexcept -> const_iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(noexcept(vector.crbegin()));
        REQUIRE(vector.crbegin() == vector.crend());
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<double> vector{{-14.0, -3.14}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(*vector.crbegin() == -3.14); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
}

TEST_CASE("crend() const noexcept -> const_iterator [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<int> vector{{-99, 100, 1000}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(noexcept(vector.crend()));
    REQUIRE(vector.crbegin() != vector.crend());
}

TEST_CASE("emplace(const_iterator before, T &&value) -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<float> vector{testFile.file()};
        REQUIRE(*vector.emplace(vector.cend(), -3.33F) == -3.33F); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(vector == std::array<float, 1>{-3.33F}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'L', '\'', 'e', 's'}, testFile.file()};
        REQUIRE(**vector.emplace(vector.begin() + 3, 'a') == 'a');
        REQUIRE(vector == std::array<char, 5>{'L', '\'', 'e', 'a', 's'});
    }
}

TEST_CASE("emplace_back(T &&value) -> reference [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(*vector.emplace_back('S') == 'S');
        REQUIRE(vector == std::array<char, 1>{'S'});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<int> vector{{1, 2, 3, 4}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        vector.emplace_back(5) = 6; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(vector == std::array<int, 5>{1, 2, 3, 4, 6});
    }
}

TEST_CASE("empty() const noexcept -> bool [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<float> vector{testFile.file()};
        REQUIRE(noexcept(vector.empty()));
        REQUIRE(vector.empty() == true);
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<char> vector{{'Y', 'a', 's', 'o', 'r'}, testFile.file()};
        REQUIRE(vector.empty() == false);
    }
}

TEST_CASE("end() noexcept -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;
    afile::PersistentVector<int> vector{{-1000, -10000, -100000}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(noexcept(vector.end()));
    REQUIRE(vector.begin() != vector.end());
}

TEST_CASE("end() const noexcept -> const_iterator [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<double> vector{{-10.34, -1.0001, 0.0, 9.9900000}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(noexcept(vector.end()));
    REQUIRE(vector.begin() != vector.end());
}

TEST_CASE("erase(const_iterator it) -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;
    afile::PersistentVector<int> vector{{-57, -654, -2, 0, -45, -58}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(*vector.erase(vector.begin() + 3) == -45);
    REQUIRE(vector == std::array<int, 5>{-57, -654, -2, -45, -58});
}

TEST_CASE("erase(const_iterator first, const_iterator last) -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(vector.erase(vector.begin(), vector.end()) == vector.end());
    }

    SECTION("[data]")
    {
        afile::PersistentVector<double> vector{{1.0, -15.98, -8.4, 5.5, 0.100, 10.1, 6.6}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        SECTION("[valid range]")
        {
            REQUIRE(*vector.erase(vector.begin() + 1, vector.begin() + 4) == 0.100); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(vector == std::array<double, 4>{1.0, 0.100, 10.1, 6.6}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("[empty range]")
        {
            REQUIRE(vector.erase(vector.begin() + 2, vector.begin() + 2) == vector.begin() + 2);
            REQUIRE(vector == std::array<double, 7>{1.0, -15.98, -8.4, 5.5, 0.100, 10.1, 6.6}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("[at end]")
        {
            REQUIRE(vector.erase(vector.begin() + 3, vector.begin() + 7) == vector.begin() + 3);
            REQUIRE(vector == std::array<double, 3>{1.0, -15.98, -8.4}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }
    }
}

TEST_CASE("file() const noexcept -> File * [afile::PersistentVector]")
{
    const TestFile testFile;
    REQUIRE(afile::PersistentVector<int>{testFile.file()}.file() == testFile.file());
}

TEST_CASE("fileIndex() const noexcept -> size_type [afile::PersistentVector]")
{
    const TestFile testFile;
    REQUIRE(afile::PersistentVector<int>{testFile.file()}.fileIndex() == 0);
}

TEST_CASE("front() -> reference [afile::PersistentVector]")
{
    const TestFile testFile;
    afile::PersistentVector<int> vector{{5, 4, 3, 2, 1, 0}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    vector.front() = 10; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(vector == std::array<int, 6>{10, 4, 3, 2, 1, 0});
}

TEST_CASE("front() const -> const_reference [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<double> vector{{9.999, -4.25}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(vector.front() == 9.999); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
}

TEST_CASE("insert(const_iterator before, const T &value) -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        const char value = 'v';
        REQUIRE(**vector.insert(vector.end(), value) == 'v');
        REQUIRE(vector == std::array<char, 1>{'v'});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<int> vector{{23, 1, 15, 1, 68}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const int value = 10;
        REQUIRE(*vector.insert(vector.begin() + 3, value) == 10);
        REQUIRE(vector == std::array<int, 6>{23, 1, 15, 10, 1, 68});
    }
}

TEST_CASE("insert(const_iterator before, T &&value) -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(**vector.insert(vector.end(), 'M') == 'M');
        REQUIRE(vector == std::array<char, 1>{'M'});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'L', '\'', 'c', 'm', 's'}, testFile.file()};
        REQUIRE(**vector.insert(vector.begin() + 2, 'r') == 'r');
        REQUIRE(vector == std::array<char, 6>{'L', '\'', 'r', 'c', 'm', 's'});
    }
}

TEST_CASE("insert(const_iterator pos, size_type count, const T &value) -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<int> vector{testFile.file()};
        const int value = 10;
        REQUIRE(*vector.insert(vector.begin(), 3, value) == 10);
        REQUIRE(vector == std::array<int, 3>{10, 10, 10});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'e', 's'}, testFile.file()};
        const char value{};
        REQUIRE(**vector.insert(vector.begin() + 1, 2, value) == char{});
        REQUIRE(vector == std::array<char, 4>{'e', char{}, char{}, 's'});
    }
}

TEST_CASE("insert(const_iterator pos, It first, It last) -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        std::array<char, 3> container{'H', 't', 'm'};
        REQUIRE(**vector.insert(vector.end(), container.begin(), container.end()) == 'H');
        REQUIRE(vector == std::array<char, 3>{'H', 't', 'm'});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<int> vector{{-1, -2, -3, -4, -10}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        std::array<int, 5> container{-5, -6, -7, -8, -9}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(*vector.insert(vector.end() - 1, container.begin(), container.end()));
        REQUIRE(vector == std::array<int, 10>{-1, -2, -3, -4, -5, -6, -7, -8, -9, -10});
    }
}

TEST_CASE("insert(const_iterator before, std::initializer_list<T> list) -> iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(**vector.insert(vector.end(), {'H', 't', 'm'}) == 'H');
        REQUIRE(vector == std::array<char, 3>{'H', 't', 'm'});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<int> vector{{-1, -2, -3, -4, -10}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(*vector.insert(vector.end() - 1, {-5, -6, -7, -8, -9}));
        REQUIRE(vector == std::array<int, 10>{-1, -2, -3, -4, -5, -6, -7, -8, -9, -10});
    }
}

TEMPLATE_TEST_CASE("max_size() const noexcept -> size_type [afile::PersistentVector]", "", int, std::string) //NOLINT(modernize-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic, cert-err58-cpp)
{
    const TestFile testFile;
    const afile::PersistentVector<TestType> vector{testFile.file()};
    REQUIRE(vector.max_size());
    REQUIRE(vector.max_size() == std::numeric_limits<acore::size_type>::max());
}

TEST_CASE("pop_back() -> void [afile::PersistentVector]")
{
    const TestFile testFile;
    afile::PersistentVector<int> vector{{6584, -6584, 9874, 3652, -4514}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    vector.pop_back();
    REQUIRE(vector == std::array<int, 4>{6584, -6584, 9874, 3652});
}

TEST_CASE("push_back(T &&value) -> void [afile::PersistentVector]")
{
    const TestFile testFile;
    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        vector.push_back('!');
        REQUIRE(vector == std::array<char, 1>{'!'});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'H', 't'}, testFile.file()};
        vector.push_back('!');
        REQUIRE(vector == std::array<char, 3>{'H', 't', '!'});
    }
}

TEST_CASE("push_back(const T &value) -> void [afile::PersistentVector]")
{
    const TestFile testFile;
    SECTION("[empty]")
    {
        afile::PersistentVector<int> vector{testFile.file()};
        const int value = 77;
        vector.push_back(value);
        REQUIRE(vector == std::array<int, 1>{77});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<float> vector{{11.35F, -12.0F, 166.1796F}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const float value = -0.31F; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        vector.push_back(value);
        REQUIRE(vector == std::array<float, 4>{11.35F, -12.0F, 166.1796F, -0.31F}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }
}

TEST_CASE("rbegin() noexcept -> reverse_iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(noexcept(vector.rbegin()));
        REQUIRE(vector.rbegin() == vector.rend());
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'G', '9', '8', 'F', 'x'}, testFile.file()};
        *vector.rbegin() = 'A';
        REQUIRE(vector == std::array<char, 5>{'G', '9', '8', 'F', 'A'});
    }
}

TEST_CASE("rbegin() const noexcept -> const_reverse_iterator [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<char> vector{testFile.file()};
        REQUIRE(noexcept(vector.rbegin()));
        REQUIRE(vector.rbegin() == vector.rend());
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<int> vector{{-2, 2}, testFile.file()};
        REQUIRE(*vector.rbegin() == 2);
    }
}

TEST_CASE("rend() noexcept -> reverse_iterator [afile::PersistentVector]")
{
    const TestFile testFile;
    afile::PersistentVector<int> vector{{-1000, -10000, -100000}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(noexcept(vector.rend()));
    REQUIRE(vector.rbegin() != vector.rend());
}

TEST_CASE("rend() const noexcept -> const_reverse_iterator [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<double> vector{{-10.34, -1.0001, 0.0, 9.9900000}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(noexcept(vector.rend()));
    REQUIRE(vector.rbegin() != vector.rend());
}

TEST_CASE("reserve(size_type size) -> void [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<int> vector{testFile.file()};
        vector.reserve(5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(vector.capacity() == 5);
        REQUIRE(vector == std::array<int, 0>{});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<float> vector{{1.0F, 1.1F, 1.3F}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        SECTION("[current]")
        {
            vector.reserve(3);
            REQUIRE(vector.capacity() == 3);
            REQUIRE(vector == std::array<float, 3>{1.0F, 1.1F, 1.3F}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("[greater]")
        {
            vector.reserve(5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(vector.capacity() == 5);
            REQUIRE(vector == std::array<float, 3>{1.0F, 1.1F, 1.3F}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("[smaller]")
        {
            vector.reserve(1);
            REQUIRE(vector.capacity() == 3);
            REQUIRE(vector == std::array<float, 3>{1.0F, 1.1F, 1.3F}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }
    }
}

TEST_CASE("resize(size_type newSize) -> void [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        SECTION("[empty]")
        {
            afile::PersistentVector<char> vector{testFile.file()};
            vector.resize(5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(vector.capacity() == 5);
            REQUIRE(vector == std::array<char, 5>{});
        }

        SECTION("[data]")
        {
            afile::PersistentVector<int> vector{{-1, 2, -3, 4, -5}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

            SECTION("[current]")
            {
                vector.resize(5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                REQUIRE(vector.capacity() == 5);
                REQUIRE(vector == std::array<int, 5>{-1, 2, -3, 4, -5});
            }

            SECTION("[greater]")
            {
                vector.resize(9); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                REQUIRE(vector.capacity() == 9);
                REQUIRE(vector == std::array<int, 9>{-1, 2, -3, 4, -5, 0, 0, 0, 0});
            }

            SECTION("[smaller]")
            {
                vector.resize(3);
                REQUIRE(vector.capacity() == 5);
                REQUIRE(vector == std::array<int, 3>{-1, 2, -3});
            }
        }
    }
}

TEST_CASE("resize(size_type newSize, const T &value) -> void [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        SECTION("[empty]")
        {
            afile::PersistentVector<double> vector{testFile.file()};
            vector.resize(6, 2.0); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(vector.capacity() == 6);
            REQUIRE(vector == std::array<double, 6>{2.0, 2.0, 2.0, 2.0, 2.0, 2.0}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("[data]")
        {
            afile::PersistentVector<int> vector{{58, 64, 82, 98, 102}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

            SECTION("[current]")
            {
                vector.resize(5, 1); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                REQUIRE(vector.capacity() == 5);
                REQUIRE(vector == std::array<int, 5>{58, 64, 82, 98, 102});
            }

            SECTION("[greater]")
            {
                vector.resize(9, 10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                REQUIRE(vector.capacity() == 9);
                REQUIRE(vector == std::array<int, 9>{58, 64, 82, 98, 102, 10, 10, 10, 10});
            }

            SECTION("[smaller]")
            {
                vector.resize(3, 100); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                REQUIRE(vector.capacity() == 5);
                REQUIRE(vector == std::array<int, 3>{58, 64, 82});
            }
        }
    }
}

TEST_CASE("shrink_to_fit() -> void [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<float> vector{testFile.file()};
        vector.shrink_to_fit();
        REQUIRE(vector.capacity() == 0);
        REQUIRE(vector == std::array<float, 0>{});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'0', '5', '-', '8', '2'}, testFile.file()};

        SECTION("[at capacity]")
        {
            vector.shrink_to_fit();
            REQUIRE(vector.capacity() == 5);
            REQUIRE(vector == std::array<char, 5>{'0', '5', '-', '8', '2'});
        }

        SECTION("[below capacity]")
        {
            vector.push_back(char{});
            REQUIRE(vector.capacity() == 8);
            vector.shrink_to_fit();
            REQUIRE(vector.capacity() == 6);
            REQUIRE(vector == std::array<char, 6>{'0', '5', '-', '8', '2', char{}});
        }
    }
}

TEST_CASE("size() const noexcept -> size_type [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<double> vector{testFile.file()};
        REQUIRE(noexcept(vector.size()));
        REQUIRE(vector.size() == 0); //NOLINT(readability-container-size-empty)
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<char> vector{{'t', 'i', 'a', 's', 't'}, testFile.file()};
        REQUIRE(vector.size() == 5);
    }
}

TEST_CASE("operator[](size_type index) -> reference [afile::PersistentVector]")
{
    const TestFile testFile;
    afile::PersistentVector<int> vector{{-25, -20, -30}, testFile.file()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    vector[1] = 10; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    REQUIRE(vector == std::array<int, 3>{-25, 10, -30});
}

TEST_CASE("operator[](size_type index) const -> const_reference [afile::PersistentVector]")
{
    const TestFile testFile;
    const afile::PersistentVector<char> vector{{'H', 'W', '!', '\n'}, testFile.file()};
    REQUIRE(*vector.at(2) == '!');
}

TEST_CASE("operator=(VectorType &&other) noexcept -> Vector & [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        afile::PersistentVector<int> vector{testFile.file()};
        afile::PersistentVector<int> other{testFile.file()};
        REQUIRE(noexcept(other = std::move(vector))); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
        other = std::move(vector); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
        REQUIRE(other == std::array<int, 0>{});
    }

    SECTION("[data]")
    {
        afile::PersistentVector<char> vector{{'A', 'B', 'C', 'D', 'E'}, testFile.file()};
        afile::PersistentVector<char> other{testFile.file()};
        other = std::move(vector);
        REQUIRE(other == std::array<char, 5>{'A', 'B', 'C', 'D', 'E'});
    }
}

TEST_CASE("operator==(const PersistentVector<T> &left, const Container &right) -> bool [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<char> left{testFile.file()};
        const afile::PersistentVector<char> right{testFile.file()};
        REQUIRE(left == right);
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<int> vector{{2, 4, 8, 16, 32}, testFile.file()};

        SECTION("[equal]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32}, testFile.file()};
            REQUIRE(vector == other);
        }

        SECTION("[different]")
        {
            const afile::PersistentVector<int> other{{32, 16, 8, 4, 2}, testFile.file()};
            REQUIRE_FALSE(vector == other);
        }

        SECTION("[shorter]")
        {
            const afile::PersistentVector<int> other{testFile.file()};
            REQUIRE_FALSE(vector == other);
        }

        SECTION("[longer]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32, 64, 128}, testFile.file()};
            REQUIRE_FALSE(vector == other);
        }
    }
}

TEST_CASE("operator!=(const Vector<T> &left, const Container &right) -> bool [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<char> left{testFile.file()};
        const afile::PersistentVector<char> right{testFile.file()};
        REQUIRE_FALSE(left != right);
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<double> vector{{-1.0, -3.9, -2.1}, testFile.file()};

        SECTION("[equal]")
        {
            const afile::PersistentVector<double> other{{-1.0, -3.9, -2.1}, testFile.file()};
            REQUIRE_FALSE(vector != other);
        }

        SECTION("[different]")
        {
            const afile::PersistentVector<double> other{{3.14, 7.1, -1.0}, testFile.file()};
            REQUIRE(vector != other);
        }

        SECTION("[shorter]")
        {
            const afile::PersistentVector<double> other{{-1.0, -3.9}, testFile.file()};
            REQUIRE(vector != other);
        }

        SECTION("[longer]")
        {
            const afile::PersistentVector<double> other{{-1.0, -3.9, -2.1, 15.15, 7.999}, testFile.file()};
            REQUIRE(vector != other);
        }
    }
}

TEST_CASE("operator<(const Vector<T> &left, const Vector<T> &right) -> bool [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<int> left{testFile.file()};
        const afile::PersistentVector<int> right{testFile.file()};
        REQUIRE_FALSE(left < right);
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<int> vector{{2, 4, 8, 16, 32}, testFile.file()};

        SECTION("[equal]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32}, testFile.file()};
            REQUIRE_FALSE(vector < other);
        }

        SECTION("[shorter]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8}, testFile.file()};
            REQUIRE_FALSE(vector < other);
        }

        SECTION("[longer]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32, 64, 128}, testFile.file()};
            REQUIRE(vector < other);
        }

        SECTION("[lesser]")
        {
            const afile::PersistentVector<int> other{{1, 5, 9, 17, 33}, testFile.file()};
            REQUIRE_FALSE(vector < other);
        }

        SECTION("[greater]")
        {
            const afile::PersistentVector<int> other{{3, 3, 7, 15, 31}, testFile.file()};
            REQUIRE(vector < other);
        }
    }
}

TEST_CASE("operator<=(const Vector<T> &left, const Vector<T> &right) -> bool [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<int> left{testFile.file()};
        const afile::PersistentVector<int> right{testFile.file()};
        REQUIRE(left <= right);
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<int> vector{{2, 4, 8, 16, 32}, testFile.file()};

        SECTION("[equal]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32}, testFile.file()};
            REQUIRE(vector <= other);
        }

        SECTION("[shorter]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8}, testFile.file()};
            REQUIRE_FALSE(vector <= other);
        }

        SECTION("[longer]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32, 64, 128}, testFile.file()};
            REQUIRE(vector <= other);
        }

        SECTION("[lesser]")
        {
            const afile::PersistentVector<int> other{{1, 5, 9, 17, 33}, testFile.file()};
            REQUIRE_FALSE(vector <= other);
        }

        SECTION("[greater]")
        {
            const afile::PersistentVector<int> other{{3, 3, 7, 15, 31}, testFile.file()};
            REQUIRE(vector <= other);
        }
    }
}

TEST_CASE("operator>(const Vector<T> &left, const Vector<T> &right) -> bool [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<int> left{testFile.file()};

        const afile::PersistentVector<int> right{testFile.file()};
        REQUIRE_FALSE(left > right);
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<int> vector{{2, 4, 8, 16, 32}, testFile.file()};

        SECTION("[equal]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32}, testFile.file()};
            REQUIRE_FALSE(vector > other);
        }

        SECTION("[shorter]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8}, testFile.file()};
            REQUIRE(vector > other);
        }

        SECTION("[longer]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32, 64, 128}, testFile.file()};
            REQUIRE_FALSE(vector > other);
        }

        SECTION("[lesser]")
        {
            const afile::PersistentVector<int> other{{1, 5, 9, 17, 33}, testFile.file()};
            REQUIRE(vector > other);
        }

        SECTION("[greater]")
        {
            const afile::PersistentVector<int> other{{3, 3, 7, 15, 31}, testFile.file()};
            REQUIRE_FALSE(vector > other);
        }
    }
}

TEST_CASE("operator>=(const Vector &left, const Vector &right) -> bool [afile::PersistentVector]")
{
    const TestFile testFile;

    SECTION("[empty]")
    {
        const afile::PersistentVector<int> left{testFile.file()};
        const afile::PersistentVector<int> right{testFile.file()};
        REQUIRE(left >= right);
    }

    SECTION("[data]")
    {
        const afile::PersistentVector<int> vector{{2, 4, 8, 16, 32}, testFile.file()};

        SECTION("[equal]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32}, testFile.file()};
            REQUIRE(vector >= other);
        }

        SECTION("[shorter]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8}, testFile.file()};
            REQUIRE(vector >= other);
        }

        SECTION("[longer]")
        {
            const afile::PersistentVector<int> other{{2, 4, 8, 16, 32, 64, 128}, testFile.file()};
            REQUIRE_FALSE(vector >= other);
        }

        SECTION("[lesser]")
        {
            const afile::PersistentVector<int> other{{1, 5, 9, 17, 33}, testFile.file()};
            REQUIRE(vector >= other);
        }

        SECTION("[greater]")
        {
            const afile::PersistentVector<int> other{{3, 3, 7, 15, 31}, testFile.file()};
            REQUIRE_FALSE(vector >= other);
        }
    }
}
}
