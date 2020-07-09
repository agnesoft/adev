#include "pch.hpp"

#include <catch2/catch.hpp>

#include <filesystem>

//namespace acore
//{
//[[nodiscard]] bool operator==(const DataIndexMap &left, const DataIndexMap &right)
//{
//    return std::equal(left.begin(), left.end(), right.begin());
//}
//}

//namespace afile
//{
//[[nodiscard]] bool operator==(const PersistentDataIndexMap &map, const acore::Vector<std::pair<size_type, acore::DataIndexMapElement>> &values)
//{
//    if(values.size() != map.count())
//    {
//        return false;
//    }

//    for(const auto &keyValue : map)
//    {
//        if(!values.contains({keyValue.key, keyValue.value}))
//        {
//            return false;
//        }
//    }

//    return true;
//}
//}

//namespace Catch
//{
//template<>
//struct StringMaker<afile::PersistentDataIndexMap>
//{
//    [[nodiscard]] static std::string convert(const afile::PersistentDataIndexMap &hash)
//    {
//        std::stringstream os;
//        os << '{';
//        for(auto keyValue : hash)
//        {
//            os << " {" << keyValue.key << '|' << (*keyValue.value).key << ", " << (*keyValue.value).value << "} ";
//        }
//        os << '}';
//        return os.str();
//    }
//};

//template<>
//struct StringMaker<acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>>
//{
//    [[nodiscard]] static std::string convert(const acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &values)
//    {
//        std::stringstream os;
//        os << '{';
//        for(auto keyValue : values)
//        {
//            os << " {" << keyValue.first << '|' << keyValue.second.key << ',' << keyValue.second.value << "} ";
//        }
//        os << '}';
//        return os.str();
//    }
//};
//}

//namespace persistentdataindexmaptest
//{
//static constexpr const char *TEST_FILE = "afile.persistentdataindexmap.testfile";

//class TestFile
//{
//public:
//    TestFile()
//    {
//        removeFiles();
//        mFile = std::make_unique<afile::File>(TEST_FILE);
//    }

//    TestFile(const acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &data) :
//        TestFile()
//    {
//        afile::PersistentDataIndexMap map{file()};
//        mIndex = map.fileIndex();

//        for(auto keyValue : data)
//        {
//            map.insert(keyValue.first, keyValue.second.key, keyValue.second.value);
//        }
//    }

//    TestFile(const TestFile &other) = delete;
//    TestFile(TestFile &&other) = delete;

//    ~TestFile() noexcept
//    {
//        try
//        {
//            try
//            {
//                mFile.reset();
//                removeFiles();
//            }
//            catch(...)
//            {
//                INFO("Unable to remove file in persistentdataindexmaptest::TestFile::~TestFile()");
//            }
//        }
//        catch(...)
//        {
//            std::terminate();
//        }
//    }

//    [[nodiscard]] afile::File *file() noexcept
//    {
//        return mFile.get();
//    }

//    [[nodiscard]] acore::size_type index() const noexcept
//    {
//        return mIndex;
//    }

//    TestFile &operator=(const TestFile &other) = delete;
//    TestFile &operator=(TestFile &&other) = delete;

//private:
//    void removeFiles()
//    {
//        if(std::filesystem::exists(TEST_FILE))
//        {
//            std::filesystem::remove(TEST_FILE);
//        }

//        if(std::filesystem::exists(std::string{"."} + TEST_FILE))
//        {
//            std::filesystem::remove(std::string{"."} + TEST_FILE);
//        }
//    }

//    std::unique_ptr<afile::File> mFile;
//    acore::size_type mIndex = 0;
//};

//TEST_CASE("PersistentDataIndexMap(File *file) [afile::PersistentDataIndexMap]")
//{
//    TestFile testFile;
//    const afile::PersistentDataIndexMap map{testFile.file()};
//    REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
//}

//TEST_CASE("PersistentDataIndexMap(File *file, size_type index) [afile::PersistentDataIndexMap]")
//{
//    acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>> data{{1, {2, 3}}, {5, {6, 7}}, {10, {4, 3}}, {1, {4, 9}}, {1, {0, 2}}};
//    TestFile testFile{data};
//    const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//    REQUIRE(map == data);
//}

//TEST_CASE("PersistentDataIndexMap(PersistentDataIndexMap &&other) noexcept -> PersistentDataIndexMap & [afile::PersistentDataIndexMap]")
//{
//    TestFile testFile{{{1, {2, 3}}, {100, {3, 5}}}};
//    afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//    const afile::PersistentDataIndexMap other{std::move(map)};
//    REQUIRE(other == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}, {100, {3, 5}}});
//}

//TEST_CASE("begin() const -> const_iterator [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(map.begin() == map.end());
//    }

//    SECTION("[data]")
//    {
//        const acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>> data{{1, {2, 3}}, {5, {6, 7}}, {10, {4, 3}}, {1, {4, 9}}, {1, {0, 2}}};
//        TestFile testFile{data};
//        const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//        const auto it = map.begin();
//        REQUIRE(it != map.end());
//        REQUIRE(data.contains({(*it).key, (*it).value}));
//    }
//}

//TEST_CASE("cbegin() const -> const_iterator [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(map.cbegin() == map.cend());
//    }

//    SECTION("[data]")
//    {
//        const acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>> data{{10, {2, 1}}, {78, {6, 55}}, {11, {20, 3}}, {2, {4, 99}}, {1, {0, 2}}, {100, {0, 1}}, {5, {-1, -5}}};
//        TestFile testFile{data};
//        const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//        const auto it = map.cbegin();
//        REQUIRE(it != map.cend());
//        REQUIRE(data.contains({(*it).key, (*it).value}));
//    }
//}

//TEST_CASE("cend() const -> const_iterator [afile::PersistentDataIndexMap]")
//{
//    TestFile testFile{{{1, {2, 3}}}};
//    const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//    REQUIRE(map.end() != map.begin());
//}

//TEST_CASE("clear() -> void [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        afile::PersistentDataIndexMap map{testFile.file()};
//        map.clear();
//        REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{1, {10, 11}}, {-1, {-2, -3}}, {5, {55, 555}}, {69, {42, 1}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//        map.clear();
//        REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
//    }
//}

//TEST_CASE("contains(size_type element, size_type key) const -> bool [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE_FALSE(map.contains(1, 2));
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{-5, {12, 12}}, {-6, {-5, 3}}, {1, {2, 3}}, {-9, {-10, -11}}, {-15, {-20, 23}}, {20, {20}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

//        SECTION("[existing]")
//        {
//            REQUIRE(std::as_const(map).contains(1, 2));
//        }

//        SECTION("[missing]")
//        {
//            REQUIRE_FALSE(std::as_const(map).contains(100, 1000));
//        }

//        SECTION("[removed]")
//        {
//            map.remove(-9);
//            REQUIRE_FALSE(std::as_const(map).contains(-9, -10));
//        }
//    }
//}

//TEST_CASE("count() const noexcept -> size_type [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(noexcept(map.count()));
//        REQUIRE(map.count() == 0);
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{51, {1, 2}}, {49, {3, 45}}, {42, {65, 7}}, {-1, {-1, -1}}, {5, {2, 3}}}};
//        const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//        REQUIRE(map.count() == 5);
//    }
//}

//TEST_CASE("count(size_type element) const [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(map.count(5) == 0);
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{1, {2, 3}}, {1, {4, 5}}, {5, {6, 7}}, {10, {11, 12}}, {1, {6, 7}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

//        SECTION("[existing]")
//        {
//            REQUIRE(std::as_const(map).count(1) == 3);
//        }

//        SECTION("[missing]")
//        {
//            REQUIRE(std::as_const(map).count(100) == 0);
//        }

//        SECTION("[removed]")
//        {
//            map.remove(5);
//            REQUIRE(std::as_const(map).count(5) == 0);
//        }
//    }
//}

//TEST_CASE("end() const -> const_iterator [afile::PersistentDataIndexMap]")
//{
//    TestFile testFile{{{0, {2, 30}}, {11, {52, 6}}}};
//    const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//    REQUIRE(map.end() != map.begin());
//}

//TEST_CASE("file() const noexcept -> File * [afile::PersistentDataIndexMap]")
//{
//    TestFile testFile;
//    REQUIRE(afile::PersistentDataIndexMap{testFile.file()}.file() == testFile.file());
//}

//TEST_CASE("fileIndex() const noexcept -> size_type [afile::PersistentDataIndexMap]")
//{
//    TestFile testFile;
//    REQUIRE(afile::PersistentDataIndexMap{testFile.file()}.fileIndex() == 0);
//}

//TEST_CASE("find(size_type element, size_type key) const -> const_iterator [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(map.find(5, 15) == map.end());
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{51, {1, 2}}, {49, {3, 45}}, {42, {65, 7}}, {-1, {-1, -1}}, {5, {2, 3}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

//        SECTION("[existing]")
//        {
//            const auto it = std::as_const(map).find(49, 3);
//            REQUIRE(it != std::as_const(map).end());
//            REQUIRE((*it).key == 49);
//            REQUIRE((*(*it).value).key == 3);
//            REQUIRE((*(*it).value).value == 45);
//        }

//        SECTION("[missing]")
//        {
//            REQUIRE(std::as_const(map).find(5, 15) == std::as_const(map).end());
//        }

//        SECTION("[removed]")
//        {
//            map.remove(42);
//            REQUIRE(std::as_const(map).find(42, 65) == std::as_const(map).end());
//        }
//    }
//}

//TEST_CASE("insert(size_type element, size_type key, size_type value) -> void [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        afile::PersistentDataIndexMap map{testFile.file()};
//        map.insert(1, 2, 3);
//        REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}});
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{100, {99, 98}}, {99, {98, 97}}, {6, {7, 8}}, {9, {10, 11}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

//        SECTION("[existing]")
//        {
//            map.insert(99, 98, 100);
//            REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{100, {99, 98}}, {99, {98, 100}}, {6, {7, 8}}, {9, {10, 11}}});
//        }

//        SECTION("[missing]")
//        {
//            map.insert(69, 42, 0);
//            REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{69, {42, 0}}, {100, {99, 98}}, {99, {98, 97}}, {6, {7, 8}}, {9, {10, 11}}});
//        }
//    }
//}

//TEST_CASE("isEmpty() const noexcept -> bool [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(noexcept(map.isEmpty()));
//        REQUIRE(map.isEmpty());
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{3, {30, 33}}}};
//        const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//        REQUIRE_FALSE(map.isEmpty());
//    }
//}

//TEST_CASE("keys() const -> Vector<size_type> [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(map.keys(1) == acore::Vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{1, {2, 3}}, {5, {6, 7}}, {10, {15, 16}}, {5, {16, 17}}, {18, {19, 20}}, {5, {1, 0}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

//        SECTION("[existing]")
//        {
//            acore::Vector<acore::size_type> actual = std::as_const(map).keys(5);
//            std::sort(actual.begin(), actual.end());
//            REQUIRE(actual == acore::Vector<acore::size_type>{1, 6, 16});
//        };

//        SECTION("[missing]")
//        {
//            REQUIRE(std::as_const(map).keys(100) == acore::Vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
//        }

//        SECTION("[removed]")
//        {
//            map.remove(18);
//            REQUIRE(std::as_const(map).keys(18) == acore::Vector<acore::size_type>{}); //NOLINT(readability-container-size-empty)
//        }
//    }
//}

//TEST_CASE("remove(size_type element) -> void [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        afile::PersistentDataIndexMap map{testFile.file()};
//        map.remove(10);
//        REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{1, {2, 3}}, {10, {2, 3}}, {100, {3, 5}}, {10, {3, 4}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

//        SECTION("[existing]")
//        {
//            map.remove(1);
//            REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{10, {2, 3}}, {100, {3, 5}}, {10, {3, 4}}});
//        }

//        SECTION("[multiple]")
//        {
//            map.remove(10);
//            REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}, {100, {3, 5}}});
//        }
//    }
//}

//TEST_CASE("remove(size_type element, size_type key) -> void [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        afile::PersistentDataIndexMap map{testFile.file()};
//        map.remove(5, 10);
//        REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{});
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{-100, {-101, -102}}, {-200, {15, 20}}, {0, {1, 2}}, {-200, {-300, -1}}, {5, {17, 58}}, {69, {42, 0}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//        map.remove(-200, -300);
//        REQUIRE(map == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{-100, {-101, -102}}, {-200, {15, 20}}, {0, {1, 2}}, {5, {17, 58}}, {69, {42, 0}}});
//    }
//}

//TEST_CASE("storage() noexcept -> Data * [afile::PersistentDataIndexMap]")
//{
//    TestFile testFile;
//    afile::PersistentDataIndexMap map{testFile.file()};
//    REQUIRE(noexcept(map.storage()));
//    REQUIRE(map.storage() != nullptr);
//}

//TEST_CASE("storage() const noexcept -> const Data * [afile::PersistentDataIndexMap]")
//{
//    TestFile testFile;
//    afile::PersistentDataIndexMap map{testFile.file()};
//    REQUIRE(noexcept(map.storage()));
//    REQUIRE(map.storage() != nullptr);
//}

//TEST_CASE("toDataIndexMap() const -> acore::DataIndexMap [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(map.toDataIndexMap() == acore::DataIndexMap{});
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{2, {4, 8}}, {32, {64, 128}}, {2, {10, 11}}, {256, {512, 1024}}, {2048, {4096, 8192}}}};
//        const afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//        REQUIRE(map.toDataIndexMap() == acore::DataIndexMap{{{2, {4, 8}}, {32, {64, 128}}, {2, {10, 11}}, {256, {512, 1024}}, {2048, {4096, 8192}}}});
//    }
//}

//TEST_CASE("value(size_type element) -> size_type [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(map.value(1, 2) == acore::INVALID_INDEX);
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{2, {4, 8}}, {32, {64, 128}}, {2, {10, 11}}, {256, {512, 1024}}, {2048, {4096, 8192}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

//        SECTION("[existing]")
//        {
//            REQUIRE(std::as_const(map).value(2, 10) == 11);
//        }

//        SECTION("[missing]")
//        {
//            REQUIRE(std::as_const(map).value(1, 10) == acore::INVALID_INDEX);
//        }

//        SECTION("[removed]")
//        {
//            map.remove(256);
//            REQUIRE(std::as_const(map).value(256, 512) == acore::INVALID_INDEX);
//        }
//    }
//}

//TEST_CASE("values(size_type element) const -> Vector<DataIndexMapElement> [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(map.values(3) == acore::Vector<acore::DataIndexMapElement>{}); //NOLINT(readability-container-size-empty)
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{10, {20, 30}}, {11, {21, 31}}, {12, {21, 31}}, {11, {13, 14}}, {52, {65, 67}}, {11, {87, 89}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

//        SECTION("[existing]")
//        {
//            REQUIRE(std::as_const(map).values(52) == acore::Vector<acore::DataIndexMapElement>{{65, 67}});
//        }

//        SECTION("[multiple]")
//        {
//            acore::Vector<acore::DataIndexMapElement> actual = std::as_const(map).values(11);
//            std::sort(actual.begin(), actual.end());
//            REQUIRE(actual == acore::Vector<acore::DataIndexMapElement>{{13, 14}, {21, 31}, {87, 89}});
//        }

//        SECTION("[missing]")
//        {
//            REQUIRE(std::as_const(map).values(100) == acore::Vector<acore::DataIndexMapElement>{}); //NOLINT(readability-container-size-empty)
//        }

//        SECTION("[removed]")
//        {
//            map.remove(12);
//            REQUIRE(std::as_const(map).values(12) == acore::Vector<acore::DataIndexMapElement>{}); //NOLINT(readability-container-size-empty)
//        }
//    }
//}

//TEST_CASE("operator[](size_type element) const -> Vector<DataIndexMapElement> [afile::PersistentDataIndexMap]")
//{
//    SECTION("[empty]")
//    {
//        TestFile testFile;
//        const afile::PersistentDataIndexMap map{testFile.file()};
//        REQUIRE(map[3] == acore::Vector<acore::DataIndexMapElement>{}); //NOLINT(readability-container-size-empty)
//    }

//    SECTION("[data]")
//    {
//        TestFile testFile{{{10, {20, 30}}, {11, {21, 31}}, {12, {21, 31}}, {11, {13, 14}}, {52, {65, 67}}, {11, {87, 89}}}};
//        afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};

//        SECTION("[existing]")
//        {
//            REQUIRE(std::as_const(map)[52] == acore::Vector<acore::DataIndexMapElement>{{65, 67}});
//        }

//        SECTION("[multiple]")
//        {
//            acore::Vector<acore::DataIndexMapElement> actual = std::as_const(map)[11];
//            std::sort(actual.begin(), actual.end());
//            REQUIRE(actual == acore::Vector<acore::DataIndexMapElement>{{13, 14}, {21, 31}, {87, 89}});
//        }

//        SECTION("[missing]")
//        {
//            REQUIRE(std::as_const(map)[100] == acore::Vector<acore::DataIndexMapElement>{}); //NOLINT(readability-container-size-empty)
//        }

//        SECTION("[removed]")
//        {
//            map.remove(12);
//            REQUIRE(std::as_const(map)[12] == acore::Vector<acore::DataIndexMapElement>{}); //NOLINT(readability-container-size-empty)
//        }
//    }
//}

//TEST_CASE("operator=(PersistentDataIndexMap &&other) noexcept -> PersistentDataIndexMap & [afile::PersistentDataIndexMap]")
//{
//    TestFile testFile{{{1, {2, 3}}, {100, {3, 5}}}};
//    afile::PersistentDataIndexMap map{testFile.file(), testFile.index()};
//    afile::PersistentDataIndexMap other{testFile.file()};
//    other = std::move(map);
//    REQUIRE(other == acore::Vector<std::pair<acore::size_type, acore::DataIndexMapElement>>{{1, {2, 3}}, {100, {3, 5}}});
//}
//}
