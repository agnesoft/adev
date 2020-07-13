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
#include <filesystem>

namespace acore
{
[[nodiscard]] auto operator==(const DataIndexMap &map, const std::vector<std::pair<size_type, DataIndexMapElement>> &values) -> bool
{
    if (values.size() != static_cast<std::size_t>(map.count()))
    {
        return false;
    }

    for (size_type e = 0; e < map.size(); ++e)
    {
        for (const DataIndexMapElement &data : map.values(e))
        {
            if (std::find(values.begin(), values.end(), std::pair<size_type, DataIndexMapElement>{e, data}) == values.end())
            {
                return false;
            }
        }
    }

    return true;
}
}

namespace afile
{
[[nodiscard]] auto operator==(const PersistentDataIndexMap &map, const std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &values) -> bool
{
    if (values.size() != static_cast<std::size_t>(map.count()))
    {
        return false;
    }

    for (acore::size_type e = 0; e < map.size(); ++e)
    {
        for (const acore::DataIndexMapElement &data : map.values(e))
        {
            if (std::find(values.begin(), values.end(), std::pair<acore::size_type, acore::DataIndexMapElement>{e, data}) == values.end())
            {
                return false;
            }
        }
    }

    return true;
}
}

namespace Catch
{
template<>
struct StringMaker<afile::PersistentDataIndexMap>
{
    [[nodiscard]] static auto convert(const afile::PersistentDataIndexMap &map) -> std::string
    {
        std::stringstream os;
        os << '{';
        for (acore::size_type e = 0; e < map.size(); ++e)
        {
            for (const acore::DataIndexMapElement &data : map.values(e))
            {
                os << " {" << e << '|' << data.key << ", " << data.value << "} ";
            }
        }
        os << '}';
        return os.str();
    }
};

template<>
struct StringMaker<std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>>
{
    [[nodiscard]] static auto convert(const std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &values) -> std::string
    {
        std::stringstream os;
        os << '{';
        for (auto keyValue : values)
        {
            os << " {" << keyValue.first << '|' << keyValue.second.key << ',' << keyValue.second.value << "} ";
        }
        os << '}';
        return os.str();
    }
};
}

namespace persistentdataindexmaptest
{
static constexpr const char *TEST_FILE = "afile.persistentdataindexmap.testfile";

class TestFile
{
public:
    TestFile()
    {
        removeFiles();
        mFile = std::make_unique<afile::File>(TEST_FILE);
    }

    explicit TestFile(const std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &data) :
        TestFile()
    {
        afile::PersistentDataIndexMap map{file()};
        mIndex = map.fileIndex();

        for (auto keyValue : data)
        {
            map.insert(keyValue.first, keyValue.second.key, keyValue.second.value);
        }
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
                INFO("Unable to remove file in persistentdataindexmaptest::TestFile::~TestFile()");
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

    std::unique_ptr<afile::File> mFile;
    acore::size_type mIndex = 0;
};

TEST_CASE("[afile::PersistentDataIndexMap]")
{
    REQUIRE_FALSE(std::is_copy_constructible_v<afile::PersistentDataIndexMap>);
    REQUIRE_FALSE(std::is_copy_assignable_v<afile::PersistentDataIndexMap>);
    REQUIRE(std::is_nothrow_move_constructible_v<afile::PersistentDataIndexMap>);
    REQUIRE(std::is_nothrow_move_assignable_v<afile::PersistentDataIndexMap>);
    REQUIRE(std::is_nothrow_destructible_v<afile::PersistentDataIndexMap>);
}

TEST_CASE("PersistentDataIndexMap(File *file) [afile::PersistentDataIndexMap]")
{
    const TestFile testFile;
    const afile::PersistentDataIndexMap map{testFile.file()};
    REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
}

TEST_CASE("PersistentDataIndexMap(File *file, acore::size_type index) [afile::PersistentDataIndexMap]")
{
    std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> data{{1, {2, 3}}, {5, {6, 7}}, {10, {4, 3}}, {1, {4, 9}}, {1, {0, 2}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    TestFile testFile{data};
    const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
    REQUIRE(map == data);
}

TEST_CASE("PersistentDataIndexMap(PersistentDataIndexMap &&other) noexcept -> PersistentDataIndexMap & [afile::PersistentDataIndexMap]")
{
    TestFile testFile{{{1, {2, 3}}, {100, {3, 5}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
    const afile::PersistentDataIndexMap other{std::move(map)};
    REQUIRE(other == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}, {100, {3, 5}}});
}

TEST_CASE("clear() -> void [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        afile::PersistentDataIndexMap map{testFile.file()};
        map.clear();
        REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
    }

    SECTION("[data]")
    {
        TestFile testFile{{{1, {10, 11}}, {1, {-2, -3}}, {5, {55, 555}}, {69, {42, 1}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
        map.clear();
        REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
    }
}

TEST_CASE("count() const noexcept -> acore::size_type [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDataIndexMap map{testFile.file()};
        REQUIRE(noexcept(map.count()));
        REQUIRE(map.count() == 0);
    }

    SECTION("[data]")
    {
        TestFile testFile{{{51, {1, 2}}, {49, {3, 45}}, {42, {65, 7}}, {1, {-1, -1}}, {5, {2, 3}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
        REQUIRE(map.count() == 5);
    }
}

TEST_CASE("count(acore::size_type element) const [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDataIndexMap map{testFile.file()};
        REQUIRE(map.count(5) == 0);
    }

    SECTION("[data]")
    {
        TestFile testFile{{{1, {2, 3}}, {1, {4, 5}}, {5, {6, 7}}, {10, {11, 12}}, {1, {6, 7}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(map).count(1) == 3);
        }

        SECTION("[missing]")
        {
            REQUIRE(std::as_const(map).count(100) == 0);
        }

        SECTION("[removed]")
        {
            map.remove(5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(std::as_const(map).count(5) == 0);
        }
    }
}

TEST_CASE("file() const noexcept -> File * [afile::PersistentDataIndexMap]")
{
    const TestFile testFile;
    REQUIRE(afile::PersistentDataIndexMap{testFile.file()}.file() == testFile.file());
}

TEST_CASE("fileIndex() const noexcept -> acore::size_type [afile::PersistentDataIndexMap]")
{
    const TestFile testFile;
    REQUIRE(afile::PersistentDataIndexMap{testFile.file()}.fileIndex() == 2);
}

TEST_CASE("insert(acore::size_type element, acore::size_type key, acore::size_type value) -> void [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        afile::PersistentDataIndexMap map{testFile.file()};
        map.insert(1, 2, 3);
        REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}});
    }

    SECTION("[data]")
    {
        TestFile testFile{{{100, {99, 98}}, {99, {98, 97}}, {6, {7, 8}}, {9, {10, 11}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            map.insert(99, 98, 100); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{100, {99, 98}}, {99, {98, 100}}, {6, {7, 8}}, {9, {10, 11}}});
        }

        SECTION("[missing]")
        {
            map.insert(69, 42, 0); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{69, {42, 0}}, {100, {99, 98}}, {99, {98, 97}}, {6, {7, 8}}, {9, {10, 11}}});
        }

        SECTION("[reinsert]")
        {
            map.remove(100); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            map.remove(6); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            map.insert(5, 15, 155); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{5, {15, 155}}, {99, {98, 97}}, {9, {10, 11}}});
        }

        SECTION("[invalid]")
        {
            REQUIRE_THROWS_AS(map.insert(-1, 1, 2), acore::Exception);
        }
    }
}

TEST_CASE("isEmpty() const noexcept -> bool [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDataIndexMap map{testFile.file()};
        REQUIRE(noexcept(map.isEmpty()));
        REQUIRE(map.isEmpty());
    }

    SECTION("[data]")
    {
        TestFile testFile{{{3, {30, 33}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
        REQUIRE_FALSE(map.isEmpty());
    }
}

TEST_CASE("keys() const -> Vector<acore::size_type> [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDataIndexMap map{testFile.file()};
        REQUIRE(map.keys(1) == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[data]")
    {
        TestFile testFile{{{1, {2, 3}}, {5, {6, 7}}, {10, {15, 16}}, {5, {16, 17}}, {18, {19, 20}}, {5, {1, 0}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            std::vector<acore::size_type> actual = std::as_const(map).keys(5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            std::sort(actual.begin(), actual.end());
            REQUIRE(actual == std::vector<acore::size_type>{1, 6, 16});
        };

        SECTION("[missing]")
        {
            REQUIRE(std::as_const(map).keys(100) == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
        }

        SECTION("[removed]")
        {
            map.remove(18); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(std::as_const(map).keys(18) == std::vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
        }
    }
}

TEST_CASE("remove(acore::size_type element) -> void [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        afile::PersistentDataIndexMap map{testFile.file()};
        map.remove(10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
    }

    SECTION("[data]")
    {
        TestFile testFile{{{1, {2, 3}}, {10, {2, 3}}, {100, {3, 5}}, {10, {3, 4}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            map.remove(1);
            REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{10, {2, 3}}, {100, {3, 5}}, {10, {3, 4}}});
        }

        SECTION("[multiple]")
        {
            map.remove(10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}, {100, {3, 5}}});
        }

        SECTION("[missing]")
        {
            map.remove(4);
            REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}, {10, {2, 3}}, {100, {3, 5}}, {10, {3, 4}}});
        }

        SECTION("[invalid]")
        {
            REQUIRE_THROWS_AS(map.remove(-3), acore::Exception);
        }
    }
}

TEST_CASE("remove(acore::size_type element, acore::size_type key) -> void [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        afile::PersistentDataIndexMap map{testFile.file()};
        map.remove(5, 10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
    }

    SECTION("[data]")
    {
        TestFile testFile{{{100, {-101, -102}}, {200, {15, 20}}, {0, {1, 2}}, {200, {-300, -1}}, {5, {17, 58}}, {69, {42, 0}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

        SECTION("[single]")
        {
            afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
            map.remove(200, -300); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{100, {-101, -102}}, {200, {15, 20}}, {0, {1, 2}}, {5, {17, 58}}, {69, {42, 0}}});
        }

        SECTION("[multiple]")
        {
            afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
            map.remove(200, 15); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{100, {-101, -102}}, {200, {-300, -1}}, {0, {1, 2}}, {5, {17, 58}}, {69, {42, 0}}});
        }
    }

    SECTION("[invalid]")
    {
        const TestFile testFile;
        afile::PersistentDataIndexMap map{testFile.file()};
        REQUIRE_THROWS_AS(map.remove(-2, 1), acore::Exception);
    }
}

TEST_CASE("shrink_to_fit() -> void [acore::DataIndexMap")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        afile::PersistentDataIndexMap map{testFile.file()};
        map.shrink_to_fit();
        REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
    }

    SECTION("[data]")
    {
        const std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> data{{1, {2, 3}}, {5, {6, 7}}, {10, {4, 3}}, {1, {4, 9}}, {1, {0, 2}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const TestFile testFile{data};
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
        map.shrink_to_fit();
        REQUIRE(map == data);
    }

    SECTION("[removed]")
    {
        const TestFile testFile{{{1, {2, 3}}, {5, {6, 7}}, {10, {4, 3}}, {1, {4, 9}}, {1, {0, 2}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
        map.remove(1);
        map.shrink_to_fit();
        REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{5, {6, 7}}, {10, {4, 3}}});
    }

    SECTION("[removed at end]")
    {
        const TestFile testFile{{{1, {2, 3}}, {5, {6, 7}}, {10, {4, 3}}, {1, {4, 9}}, {1, {0, 2}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
        map.remove(10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        map.shrink_to_fit();
        REQUIRE(map == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}, {5, {6, 7}}, {1, {4, 9}}, {1, {0, 2}}});
    }
}

TEST_CASE("size() const noexcept -> acore::size_type [acore::DataIndexMap")
{

    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDataIndexMap map{testFile.file()};
        REQUIRE(noexcept(map.size()));
        REQUIRE(map.size() == 0);
    }

    SECTION("[data]")
    {
        const TestFile testFile;
        afile::PersistentDataIndexMap map{testFile.file()};
        map.insert(1, 2, 3);
        map.insert(3, 1, 2);
        map.insert(5, 4, 4); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(map.size() == 6);
    }

    SECTION("[multiple data per element]")
    {
        const TestFile testFile;
        afile::PersistentDataIndexMap map{testFile.file()};
        map.insert(1, 2, 3);
        map.insert(3, 1, 2);
        map.insert(3, 2, 2);
        map.insert(3, 3, 3);
        REQUIRE(map.size() == 4);
    }

    SECTION("[removed]")
    {
        const TestFile testFile;
        afile::PersistentDataIndexMap map{testFile.file()};
        map.insert(1, 2, 3);
        map.insert(3, 1, 2);
        map.insert(5, 4, 4); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        map.remove(5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(map.size() == 6);
    }

    SECTION("[shrinked]")
    {
        TestFile testFile{{{1, {2, 3}}, {5, {6, 7}}, {10, {4, 3}}, {1, {4, 9}}, {1, {0, 2}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        map.remove(10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        map.shrink_to_fit();
        REQUIRE(map.size() == 6);
    }
}

TEST_CASE("storage() noexcept -> Data * [afile::PersistentDataIndexMap]")
{
    const TestFile testFile;
    afile::PersistentDataIndexMap map{testFile.file()};
    REQUIRE(noexcept(map.storage()));
    REQUIRE(map.storage() != nullptr);
}

TEST_CASE("storage() const noexcept -> const Data * [afile::PersistentDataIndexMap]")
{
    const TestFile testFile;
    afile::PersistentDataIndexMap map{testFile.file()};
    REQUIRE(noexcept(map.storage()));
    REQUIRE(map.storage() != nullptr);
}

TEST_CASE("toDataIndexMap() const -> acore::DataIndexMap [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDataIndexMap map{testFile.file()};
        REQUIRE(map.toDataIndexMap() == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
    }

    SECTION("[data]")
    {
        TestFile testFile{{{2, {4, 8}}, {32, {64, 128}}, {2, {10, 11}}, {256, {512, 1024}}, {2048, {4096, 8192}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
        REQUIRE(map.toDataIndexMap() == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{{2, {4, 8}}, {32, {64, 128}}, {2, {10, 11}}, {256, {512, 1024}}, {2048, {4096, 8192}}}});
    }
}

TEST_CASE("value(acore::size_type element) -> acore::size_type [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDataIndexMap map{testFile.file()};
        REQUIRE_THROWS_AS(std::as_const(map).value(1, 2), acore::Exception);
    }

    SECTION("[data]")
    {
        TestFile testFile{{{2, {4, 8}}, {32, {64, 128}}, {2, {10, 11}}, {256, {512, 1024}}, {2048, {4096, 8192}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(map).value(2, 10) == 11);
        }

        SECTION("[missing]")
        {
            REQUIRE_THROWS_AS(std::as_const(map).value(1, 10), acore::Exception);
        }

        SECTION("[removed]")
        {
            map.remove(256); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE_THROWS_AS(std::as_const(map).value(256, 512), acore::Exception);
        }
    }
}

TEST_CASE("values(acore::size_type element) const -> Vector<DataIndexMapElement> [afile::PersistentDataIndexMap]")
{
    SECTION("[empty]")
    {
        const TestFile testFile;
        const afile::PersistentDataIndexMap map{testFile.file()};
        REQUIRE(map.values(3) == std::vector<acore::DataIndexMapElement>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[data]")
    {
        TestFile testFile{{{10, {20, 30}}, {11, {21, 31}}, {12, {21, 31}}, {11, {13, 14}}, {52, {65, 67}}, {11, {87, 89}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(map).values(52) == std::vector<acore::DataIndexMapElement>{{65, 67}});
        }

        SECTION("[multiple]")
        {
            std::vector<acore::DataIndexMapElement> actual = std::as_const(map).values(11); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            std::sort(actual.begin(), actual.end());
            REQUIRE(actual == std::vector<acore::DataIndexMapElement>{{13, 14}, {21, 31}, {87, 89}});
        }

        SECTION("[missing]")
        {
            REQUIRE(std::as_const(map).values(100) == std::vector<acore::DataIndexMapElement>{}); //NOLINT(readability-container-size-empty)
        }

        SECTION("[removed]")
        {
            map.remove(12); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(std::as_const(map).values(12) == std::vector<acore::DataIndexMapElement>{}); //NOLINT(readability-container-size-empty)
        }
    }
}

TEST_CASE("operator=(PersistentDataIndexMap &&other) noexcept -> PersistentDataIndexMap & [afile::PersistentDataIndexMap]")
{
    TestFile testFile{{{1, {2, 3}}, {100, {3, 5}}}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
    afile::PersistentDataIndexMap other{testFile.file()};
    other = std::move(map);
    REQUIRE(other == std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}, {100, {3, 5}}});
}
}
