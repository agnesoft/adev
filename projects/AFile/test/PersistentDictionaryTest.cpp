#include "pch.hpp"

#include <catch2/catch.hpp>
#include <filesystem>

namespace acore
{
[[nodiscard]] bool operator==(const Dictionary &left, const Dictionary &right)
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
        mValue(i)
    {
    }

    [[nodiscard]] bool operator==(const DuplicateValue &right) const noexcept
    {
        return mValue == right.mValue;
    }

    [[nodiscard]] bool operator<(const DuplicateValue &right) const noexcept
    {
        return mValue < right.mValue;
    }

private:
    template<typename Buffer>
    friend acore::DataStreamBase<Buffer> &operator<<(acore::DataStreamBase<Buffer> &stream, const DuplicateValue &value)
    {
        return stream << value.mValue;
    }

    template<typename Buffer>
    friend acore::DataStreamBase<Buffer> &operator>>(acore::DataStreamBase<Buffer> &stream, DuplicateValue &value)
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

    result_type operator()([[maybe_unused]] const argument_type &value) const noexcept
    {
        return 1;
    }
};
}

namespace afile
{
[[nodiscard]] bool operator==(const PersistentDictionary &dictionary, const acore::Vector<acore::Variant> &values)
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
    TestFile(Args... args) :
        TestFile()
    {
        afile::PersistentDictionary dictionary{file()};
        mIndex = dictionary.fileIndex();
        insertValues(dictionary, args...);
    }

    TestFile(const TestFile &other) = delete;
    TestFile(TestFile &&other) = delete;

    ~TestFile() noexcept
    {
        try
        {
            try
            {
                mFile.reset();
                removeFiles();
            }
            catch(...)
            {
                INFO("Unable to remove file in persistentdictionarytest::TestFile::~TestFile()");
            }
        }
        catch(...)
        {
            std::terminate();
        }
    }

    [[nodiscard]] afile::File *file() noexcept
    {
        return mFile.get();
    }

    [[nodiscard]] acore::size_type index() const noexcept
    {
        return mIndex;
    }

    TestFile &operator=(const TestFile &other) = delete;
    TestFile &operator=(TestFile &&other) = delete;

private:
    void removeFiles()
    {
        if(std::filesystem::exists(TEST_FILE))
        {
            std::filesystem::remove(TEST_FILE);
        }

        if(std::filesystem::exists(std::string{"."} + TEST_FILE))
        {
            std::filesystem::remove(std::string{"."} + TEST_FILE);
        }
    }

    template<typename T, typename... Args>
    void insertValues(afile::PersistentDictionary &dictionary, const T &value, Args... values)
    {
        dictionary.insert(value);

        if constexpr(sizeof...(values) > 0)
        {
            insertValues(dictionary, values...);
        }
    }

    std::unique_ptr<afile::File> mFile;
    acore::size_type mIndex = 0;
};

TEST_CASE("PersistentDictionary(File *file) [afile::PersistentDictionary]")
{
    TestFile testFile;
    const afile::PersistentDictionary dictionary{testFile.file()};
    REQUIRE(dictionary == acore::Vector<acore::Variant>{});
}

TEST_CASE("PersistentDictionary(File *file, size_type index) [afile::PersistentDictionary]")
{
    SECTION("[valid]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}});
    }

    SECTION("[removed values]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};

        afile::PersistentDictionary{testFile.file(), testFile.index()}.remove(2);
        testFile.file()->insert(acore::Vector<int>{1, 2, 3, 4, 5});
        const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE_FALSE(dictionary.contains(2));
        REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{acore::Vector<char>{'a', 'b'}}});
    }
}

TEST_CASE("Dictionary(Dictionary &&other) [afile::PersistentDictionary]")
{
    TestFile testFile{std::string{"Hello"},
                      acore::Vector<int>{1, 2, 3, 4, 5},
                      std::string{"Hello"},
                      std::string{"World"},
                      acore::Vector<char>{'a', 'b'}};
    afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
    const afile::PersistentDictionary other{std::move(dictionary)};
    REQUIRE(other == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}});
}

TEST_CASE("begin() const noexcept -> const_iterator [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(noexcept(dictionary.begin()));
        REQUIRE(dictionary.begin() == dictionary.end());
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE(dictionary.begin() != dictionary.end());
        REQUIRE(*dictionary.begin() == acore::Variant{std::string{"Hello"}});
    }
}

TEST_CASE("clear() -> void [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        afile::PersistentDictionary dictionary{testFile.file()};
        dictionary.clear();
        REQUIRE(dictionary == acore::Vector<acore::Variant>{});
    }

    SECTION("[data]")
    {
        TestFile testFile{acore::Vector<int>{5, 7, 9}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        dictionary.clear();
        REQUIRE(dictionary == acore::Vector<acore::Variant>{});
    }
}

TEST_CASE("contains(size_type index) const -> bool [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE_FALSE(dictionary.contains(1));
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"}, std::string{"World"}, acore::Vector<char>{'a', 'b', 'c'}};
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
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE_FALSE(dictionary.containsValue(std::string{"Helo"}));
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).containsValue(std::string{"Hello"}));
        }

        SECTION("[missing]")
        {
            REQUIRE_FALSE(std::as_const(dictionary).containsValue(acore::Vector<int>{1, 2, 3, 4}));
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
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(noexcept(dictionary.count()));
        REQUIRE(dictionary.count() == 0);
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        REQUIRE(dictionary.count() == 4);
    }
}

TEST_CASE("count(size_type index) const -> size_type [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary.count(1) == 0);
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
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
    TestFile testFile{std::string{"A"}};
    const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
    REQUIRE(noexcept(dictionary.end()));
    REQUIRE(dictionary.end() != dictionary.begin());
}

TEST_CASE("file() const noexcept -> File * [afile::PersistentDictionary]")
{
    TestFile testFile;
    REQUIRE(afile::PersistentDictionary{testFile.file()}.file() == testFile.file());
}

TEST_CASE("fileIndex() const noexcept -> size_type [afile::PersistentDictionary]")
{
    TestFile testFile;
    REQUIRE(afile::PersistentDictionary{testFile.file()}.fileIndex() == 2);
}

TEST_CASE("index(const T &value) const -> size_type [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary.index(std::string{"Hello"}) == acore::INVALID_INDEX);
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).index(std::string{"World"}) == 2);
        }

        SECTION("[missing]")
        {
            REQUIRE(std::as_const(dictionary).index(acore::Vector<std::string>{"Hello", "World"}) == acore::INVALID_INDEX);
        }

        SECTION("[removed]")
        {
            dictionary.remove(3);
            REQUIRE(std::as_const(dictionary).index(acore::Vector<char>{'a', 'b'}) == acore::INVALID_INDEX);
        }

        SECTION("[trivial]")
        {
            acore::size_type expected{};
            std::memcpy(&expected, "Hello", 5);
            REQUIRE(std::as_const(dictionary).index("Hello") == expected);
        }
    }
}

TEST_CASE("insert(const T &value) -> size_type [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary.insert(std::string{"Hello"}) == 0);
        REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}});
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(dictionary.insert(std::string{"World"}) == 2);
            REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}});
        }

        SECTION("[missing]")
        {
            REQUIRE(dictionary.insert(acore::Vector<std::string>{"Hi", "There"}) == 4);
            REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}, acore::Variant{acore::Vector<std::string>{"Hi", "There"}}});
        }

        SECTION("[removed]")
        {
            dictionary.remove(1);
            REQUIRE(dictionary.insert(std::string{"Test"}) == 1);
            REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::string{"Test"}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}, acore::Variant{acore::Vector<std::string>{"Hi", "There"}}});
        }

        SECTION("[removed multi]")
        {
            dictionary.remove(0);
            REQUIRE(dictionary.insert(std::string{"Hello"}) == 0);
            REQUIRE(dictionary.count(0) == 2);
            REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}});
        }

        SECTION("[trivial]")
        {
            acore::size_type expectedIndex{};
            std::memcpy(&expectedIndex, "Hello", 5);
            REQUIRE(dictionary.insert("Hello") == expectedIndex);
            REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}});
        }
    }
}

TEST_CASE("isEmpty() const noexcept -> bool [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(noexcept(dictionary.isEmpty()));
        REQUIRE(dictionary.isEmpty());
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Value"}};
        const afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE_FALSE(dictionary.isEmpty());
    }
}

TEST_CASE("remove(size_type index) -> void [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        afile::PersistentDictionary dictionary{testFile.file()};
        dictionary.remove(0);
        REQUIRE(dictionary == acore::Vector<acore::Variant>{});
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[single]")
        {
            dictionary.remove(3);
            REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}});
        }

        SECTION("[multi]")
        {
            dictionary.remove(0);
            REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}});
        }

        SECTION("[missing]")
        {
            dictionary.remove(10);
            REQUIRE(dictionary == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}});
        }
    }
}

TEST_CASE("storage() noexcept -> Storage * [afile::PersistentDictionary]")
{
    TestFile testFile;
    afile::PersistentDictionary dictionary{testFile.file()};
    REQUIRE(noexcept(dictionary.storage()));
    REQUIRE(dictionary.storage());
}

TEST_CASE("storage() const noexcept -> const Storage * [afile::PersistentDictionary]")
{
    TestFile testFile;
    const afile::PersistentDictionary dictionary{testFile.file()};
    REQUIRE(noexcept(dictionary.storage()));
    REQUIRE(dictionary.storage());
}

TEST_CASE("toDictionary() const -> acore::Dictionary [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary.toDictionary() == acore::Dictionary{});
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
        REQUIRE(dictionary.toDictionary() == acore::Dictionary{std::string{"Hello"}, acore::Vector<int>{1, 2, 3, 4, 5}, std::string{"Hello"}, std::string{"World"}, acore::Vector<char>{'a', 'b'}});
    }
}

TEST_CASE("value(size_type index) const -> T [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary.value<std::string>(0) == std::string{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).value<acore::Vector<int>>(1) == acore::Vector<int>{1, 2, 3, 4, 5});
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
            REQUIRE(std::as_const(dictionary).value<acore::Vector<char>>(3) == acore::Vector<char>{}); //NOLINT(readability-container-size-empty)
        }

        SECTION("[trivial]")
        {
            REQUIRE(std::as_const(dictionary).value<char>('A') == 'A');
        }
    }
}

TEST_CASE("operator[](size_type index) const -> Variant [afile::PersistentDictionary]")
{
    SECTION("[empty]")
    {
        TestFile testFile;
        const afile::PersistentDictionary dictionary{testFile.file()};
        REQUIRE(dictionary[0] == acore::Variant{});
    }

    SECTION("[data]")
    {
        TestFile testFile{std::string{"Hello"},
                          acore::Vector<int>{1, 2, 3, 4, 5},
                          std::string{"Hello"},
                          std::string{"World"},
                          acore::Vector<char>{'a', 'b'}};
        afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary)[1] == acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}});
        }

        SECTION("[multi]")
        {
            REQUIRE(std::as_const(dictionary)[0] == acore::Variant{std::string{"Hello"}});
        }

        SECTION("[multi removed]")
        {
            dictionary.remove(0);
            REQUIRE(std::as_const(dictionary)[0] == acore::Variant{std::string{"Hello"}});
        }

        SECTION("[removed]")
        {
            dictionary.remove(3);
            REQUIRE(std::as_const(dictionary)[3] == acore::Variant{});
        }

        SECTION("[trivial]")
        {
            REQUIRE(std::as_const(dictionary)['A'] == acore::Variant{});
        }
    }
}

TEST_CASE("operator=(PersistentDictionary &&other) -> PersistentDictionary & [afile::PersistentDictionary]")
{
    TestFile testFile{std::string{"Hello"},
                      acore::Vector<int>{1, 2, 3, 4, 5},
                      std::string{"Hello"},
                      std::string{"World"},
                      acore::Vector<char>{'a', 'b'}};
    afile::PersistentDictionary dictionary{testFile.file(), testFile.index()};
    afile::PersistentDictionary other{testFile.file()};
    other = std::move(dictionary);
    REQUIRE(other == acore::Vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{acore::Vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{acore::Vector<char>{'a', 'b'}}});
}
}
