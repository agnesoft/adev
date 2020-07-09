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

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace acore
{
[[nodiscard]] auto operator==(const Dictionary &left, const Dictionary &right) -> bool
{
    return std::equal(left.begin(), left.end(), right.begin());
}
}

namespace persistentdictionarytest
{
class DuplicateValue
{
public:
    DuplicateValue() = default;

    explicit DuplicateValue(int i) :
        mValue{i}
    {
    }

    [[nodiscard]] auto operator==(const DuplicateValue &right) const noexcept -> bool
    {
        return mValue == right.mValue;
    }

    [[nodiscard]] auto operator<(const DuplicateValue &right) const noexcept -> bool
    {
        return mValue < right.mValue;
    }

private:
    template<typename Buffer>
    friend auto operator<<(acore::DataStreamBase<Buffer> &stream, const DuplicateValue &value) -> acore::DataStreamBase<Buffer> &
    {
        return stream << value.mValue;
    }

    template<typename Buffer>
    friend auto operator>>(acore::DataStreamBase<Buffer> &stream, DuplicateValue &value) -> acore::DataStreamBase<Buffer> &
    {
        return stream >> value.mValue;
    }

    int mValue = 0;
    std::string mStr;
};
}

namespace std
{
template<>
struct hash<persistentdictionarytest::DuplicateValue>
{
    using argument_type = persistentdictionarytest::DuplicateValue;
    using result_type = std::size_t;

    [[nodiscard]] auto operator()([[maybe_unused]] const argument_type &value) const noexcept -> result_type
    {
        return 1;
    }
};
}

namespace afile
{
[[nodiscard]] auto operator==(const PersistentDictionary &dictionary, const std::vector<acore::Variant> &values) -> bool
{
    return std::equal(dictionary.begin(), dictionary.end(), values.begin());
}
}

namespace persistentdictionarytest
{
static constexpr const char *TEST_FILE = "afile.persistentdictionary.testfile";

class TestFile
{
public:
    TestFile()
    {
        removeFiles();
        mFile = std::make_unique<afile::File>(TEST_FILE);
    }

    template<typename... Args>
    explicit TestFile(Args... args) :
        TestFile()
    {
        afile::PersistentDictionary dictionary{file()};
        mIndex = dictionary.fileIndex();
        insertValues(dictionary, args...);
    }

    TestFile(const TestFile &other) = delete;
    TestFile(TestFile &&other) = delete;

    ~TestFile()
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
                INFO("Unable to remove file in persistentdictionarytest::TestFile::~TestFile()");
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

    [[nodiscard]] auto index() const noexcept -> acore::size_type
    {
        return mIndex;
    }

    auto operator=(const TestFile &other) -> TestFile & = delete;
    auto operator=(TestFile &&other) -> TestFile & = delete;

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

    template<typename T, typename... Args>
    auto insertValues(afile::PersistentDictionary &dictionary, const T &value, Args... values) -> void
    {
        dictionary.insert(value);

        if constexpr (sizeof...(values) > 0)
        {
            insertValues(dictionary, values...);
        }
    }

    std::unique_ptr<afile::File> mFile;
    acore::size_type mIndex = 0;
};

TEST_CASE("PersistentDictionary(File *file) [afile::PersistentDictionary]")
{
    const TestFile testFile;
    const afile::PersistentDictionary dictionary{testFile.file()};
    REQUIRE(dictionary == std::vector<acore::Variant>{});
}

TEST_CASE("PersistentDictionary(File *file, size_type index) [afile::PersistentDictionary]")
{
    SECTION("[valid]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
    }

    SECTION("[removed values]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};

        afile::PersistentDictionary{testFile.file(), testFile.index()}.remove(2);
        testFile.file()->insert(std::vector<int>{1, 2, 3, 4, 5}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE_FALSE(dictionary.contains(2));
        REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::vector<char>{'a', 'b'}}});
    }
}

TEST_CASE("Dictionary(Dictionary &&other) [afile::PersistentDictionary]")
{
    const TestFile testFile{std::string{"Hello"},
                            std::vector<int>{1, 2, 3, 4, 5},
                            std::string{"Hello"},
                            std::string{"World"},
                            std::vector<char>{'a', 'b'}};
    afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
    const afile::PersistentDictionary other{std::move(dictionary)};
    REQUIRE(other == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
}

TEST_CASE("begin() const noexcept -> const_iterator [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(noexcept(dictionary.begin()));
        REQUIRE(dictionary.begin() == dictionary.end());
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE(dictionary.begin() != dictionary.end());
        REQUIRE(*dictionary.begin() == acore::Variant{std::string{"Hello"}});
    }
}

TEST_CASE("clear() -> void [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        afile::PersistentDictionary dictionary{testFile.file()};
        dictionary.clear();
        REQUIRE(dictionary == std::vector<acore::Variant>{});
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::vector<int>{5, 7, 9}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        dictionary.clear();
        REQUIRE(dictionary == std::vector<acore::Variant>{});
    }
}

TEST_CASE("contains(size_type index) const -> bool [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE_FALSE(dictionary.contains(1));
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"}, std::string{"World"}, std::vector<char>{'a', 'b', 'c'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).contains(1));
        }

        SECTION("[missing]")
        {
            REQUIRE_FALSE(std::as_const(dictionary).contains(10));
        }

        SECTION("[removed]")
        {
            dictionary.remove(1);
            REQUIRE_FALSE(std::as_const(dictionary).contains(1));
        }
    }
}

TEST_CASE("containsValue(const T &value) const -> bool [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE_FALSE(dictionary.containsValue(std::string{"Helo"}));
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).containsValue(std::string{"Hello"}));
        }

        SECTION("[missing]")
        {
            REQUIRE_FALSE(std::as_const(dictionary).containsValue(std::vector<int>{1, 2, 3, 4}));
        }

        SECTION("[removed]")
        {
            dictionary.remove(0);
            dictionary.remove(0);
            REQUIRE_FALSE(std::as_const(dictionary).containsValue(std::string{"Hello"}));
        }

        SECTION("[trivial]")
        {
            REQUIRE_FALSE(std::as_const(dictionary).containsValue("Hello"));
        }
    }
}

TEST_CASE("count() const noexcept -> size_type [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(noexcept(dictionary.count()));
        REQUIRE(dictionary.count() == 0);
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        REQUIRE(dictionary.count() == 4);
    }
}

TEST_CASE("count(size_type index) const -> size_type [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary.count(1) == 0);
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[single]")
        {
            REQUIRE(std::as_const(dictionary).count(1) == 1);
        }

        SECTION("[multi]")
        {
            REQUIRE(std::as_const(dictionary).count(0) == 2);
        }

        SECTION("[missing]")
        {
            REQUIRE(std::as_const(dictionary).count(10) == 0);
        }

        SECTION("[removed]")
        {
            dictionary.remove(2);
            REQUIRE(std::as_const(dictionary).count(2) == 0);
        }

        SECTION("[multi removed]")
        {
            dictionary.remove(0);
            REQUIRE(std::as_const(dictionary).count(0) == 1);
        }
    }
}

TEST_CASE("end() const noexcept -> const_iterator [afile::PersistentDictionary]")
{
    const TestFile testFile{std::string{"A"}};
    const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
    REQUIRE(noexcept(dictionary.end()));
    REQUIRE(dictionary.end() != dictionary.begin());
}

TEST_CASE("file() const noexcept -> File * [afile::PersistentDictionary]")
{
    const TestFile testFile;
    REQUIRE(afile::PersistentDictionary{testFile.file()}.file() == testFile.file());
}

TEST_CASE("fileIndex() const noexcept -> size_type [afile::PersistentDictionary]")
{
    const TestFile testFile;
    REQUIRE(afile::PersistentDictionary{testFile.file()}.fileIndex() == 0);
}

TEST_CASE("index(const T &value) const -> size_type [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary.index(std::string{"Hello"}) == acore::INVALID_INDEX);
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).index(std::string{"World"}) == 2);
        }

        SECTION("[missing]")
        {
            REQUIRE(std::as_const(dictionary).index(std::vector<std::string>{"Hello", "World"}) == acore::INVALID_INDEX);
        }

        SECTION("[removed]")
        {
            dictionary.remove(3);
            REQUIRE(std::as_const(dictionary).index(std::vector<char>{'a', 'b'}) == acore::INVALID_INDEX);
        }

        SECTION("[trivial]")
        {
            acore::size_type expected{};
            std::memcpy(&expected, "Hello", 5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(std::as_const(dictionary).index("Hello") == expected);
        }
    }
}

TEST_CASE("insert(const T &value) -> size_type [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary.insert(std::string{"Hello"}) == 0);
        REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}});
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(dictionary.insert(std::string{"World"}) == 2);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }

        SECTION("[missing]")
        {
            REQUIRE(dictionary.insert(std::vector<std::string>{"Hi", "There"}) == 4);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}, acore::Variant{std::vector<std::string>{"Hi", "There"}}});
        }

        SECTION("[removed]")
        {
            dictionary.remove(1);
            REQUIRE(dictionary.insert(std::string{"Test"}) == 1);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::string{"Test"}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}, acore::Variant{std::vector<std::string>{"Hi", "There"}}});
        }

        SECTION("[removed multi]")
        {
            dictionary.remove(0);
            REQUIRE(dictionary.insert(std::string{"Hello"}) == 0);
            REQUIRE(dictionary.count(0) == 2);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }

        SECTION("[trivial]")
        {
            acore::size_type expectedIndex{};
            std::memcpy(&expectedIndex, "Hello", 5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(dictionary.insert("Hello") == expectedIndex);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }
    }
}

TEST_CASE("isEmpty() const noexcept -> bool [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(noexcept(dictionary.isEmpty()));
        REQUIRE(dictionary.isEmpty());
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Value"}};
        const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE_FALSE(dictionary.isEmpty());
    }
}

TEST_CASE("remove(size_type index) -> void [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        afile::PersistentDictionary dictionary{testFile.file()};
        dictionary.remove(0);
        REQUIRE(dictionary == std::vector<acore::Variant>{});
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[single]")
        {
            dictionary.remove(3);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}});
        }

        SECTION("[multi]")
        {
            dictionary.remove(0);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }

        SECTION("[missing]")
        {
            dictionary.remove(10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }
    }
}

TEST_CASE("storage() noexcept -> Storage * [afile::PersistentDictionary]")
{
    const TestFile testFile;
    afile::PersistentDictionary dictionary{testFile.file()};
    REQUIRE(noexcept(dictionary.storage()));
    REQUIRE(dictionary.storage());
}

TEST_CASE("storage() const noexcept -> const Storage * [afile::PersistentDictionary]")
{
    const TestFile testFile;
    const afile::PersistentDictionary dictionary{testFile.file()};
    REQUIRE(noexcept(dictionary.storage()));
    REQUIRE(dictionary.storage());
}

TEST_CASE("toDictionary() const -> acore::Dictionary [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary.toDictionary() == acore::Dictionary{});
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE(dictionary.toDictionary() == acore::Dictionary{std::string{"Hello"}, std::vector<int>{1, 2, 3, 4, 5}, std::string{"Hello"}, std::string{"World"}, std::vector<char>{'a', 'b'}});
    }
}

TEST_CASE("value(size_type index) const -> T [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE_THROWS_AS(dictionary.value<std::string>(0), acore::Exception);
    }

    SECTION("[data]")
    {
        const TestFile testFile{std::string{"Hello"},
                                std::vector<int>{1, 2, 3, 4, 5},
                                std::string{"Hello"},
                                std::string{"World"},
                                std::vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).value<std::vector<int>>(1) == std::vector<int>{1, 2, 3, 4, 5});
        }

        SECTION("[multi]")
        {
            REQUIRE(std::as_const(dictionary).value<std::string>(0) == std::string{"Hello"});
        }

        SECTION("[multi removed]")
        {
            dictionary.remove(0);
            REQUIRE(std::as_const(dictionary).value<std::string>(0) == std::string{"Hello"});
        }

        SECTION("[removed]")
        {
            dictionary.remove(3);
            REQUIRE_THROWS_AS(std::as_const(dictionary).value<std::vector<char>>(3), acore::Exception);
        }

        SECTION("[trivial]")
        {
            REQUIRE(std::as_const(dictionary).value<char>('A') == 'A');
        }
    }
}

TEST_CASE("operator=(PersistentDictionary &&other) -> PersistentDictionary & [afile::PersistentDictionary]")
{
    const TestFile testFile{std::string{"Hello"},
                            std::vector<int>{1, 2, 3, 4, 5},
                            std::string{"Hello"},
                            std::string{"World"},
                            std::vector<char>{'a', 'b'}};
    afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
    afile::PersistentDictionary other{testFile.file()};
    other = std::move(dictionary);
    REQUIRE(other == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
}
}
