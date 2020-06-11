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

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <unordered_map>
#include <utility>
#include <vector>

namespace dataindexmapbenchmark
{
class Map2DVector
{
public:
    explicit Map2DVector(const std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &data)
    {
        for (auto val : data)
        {
            if (val.first >= static_cast<acore::size_type>(mData.size()))
            {
                mData.resize(val.first + 1);
            }

            mData[val.first].push_back(val.second);
        }
    }

    [[nodiscard]] auto value(acore::size_type element, acore::size_type key) const
    {
        for (auto e : mData[element])
        {
            if (e.key == key)
            {
                return e.value;
            }
        }

        return acore::INVALID_INDEX;
    }

private:
    std::vector<std::vector<acore::DataIndexMapElement>> mData;
};

class MapHashVector
{
public:
    explicit MapHashVector(const std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &data)
    {
        for (auto val : data)
        {
            const auto it = mIndex.find(val.first);

            if (it == mIndex.end())
            {
                mIndex.insert(std::pair<acore::size_type, Element>{val.first, {val.second, acore::INVALID_INDEX}});
            }
            else
            {
                const acore::size_type next = mExtra.size();
                mExtra.emplace_back(Element{val.second, it->second.next});
                it->second.next = next;
            }
        }
    }

    [[nodiscard]] auto value(acore::size_type element, acore::size_type key) const
    {
        const auto it = mIndex.find(element);

        if (it != mIndex.end())
        {
            const auto *e = &it->second;

            while (true)
            {
                if (e->data.key == key)
                {
                    return e->data.value;
                }

                if (e->next == acore::INVALID_INDEX)
                {
                    return acore::INVALID_INDEX;
                }

                e = &mExtra[e->next];
            }
        }

        return acore::INVALID_INDEX;
    }

private:
    struct Element
    {
        acore::DataIndexMapElement data;
        acore::size_type next = acore::INVALID_INDEX;
    };

    std::unordered_map<acore::size_type, Element> mIndex;
    std::vector<Element> mExtra;
};

class MapVectorLinkedList
{
public:
    explicit MapVectorLinkedList(const std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &data)
    {
        for (auto val : data)
        {
            if (val.first >= static_cast<acore::size_type>(mIndex.size()))
            {
                mIndex.resize(val.first + 1);
            }

            if (mIndex[val.first].data == acore::DataIndexMapElement{})
            {
                mIndex[val.first].data = val.second;
            }
            else
            {
                const acore::size_type next = mExtra.size();
                mExtra.emplace_back(Element{val.second, mIndex[val.first].next});
                mIndex[val.first].next = next;
            }
        }
    }

    [[nodiscard]] auto value(acore::size_type element, acore::size_type key) const
    {
        if (element < static_cast<acore::size_type>(mIndex.size()))
        {
            const auto *e = &mIndex[element];

            while (true)
            {
                if (e->data.key == key)
                {
                    return e->data.value;
                }

                if (e->next == acore::INVALID_INDEX)
                {
                    return acore::INVALID_INDEX;
                }

                e = &mExtra[e->next];
            }
        }

        return acore::INVALID_INDEX;
    }

private:
    struct Element
    {
        acore::DataIndexMapElement data;
        acore::size_type next = acore::INVALID_INDEX;
    };

    std::vector<Element> mIndex;
    std::vector<Element> mExtra;
};

class MapHashBuckets
{
public:
    explicit MapHashBuckets(const std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &data)
    {
        for (auto val : data)
        {
            mData[val.first].push_back(val.second);
        }
    }

    [[nodiscard]] auto value(acore::size_type element, acore::size_type key) const
    {
        const auto it = mData.find(element);

        if (it != mData.end())
        {
            for (auto e : it->second)
            {
                if (e.key == key)
                {
                    return e.value;
                }
            }
        }

        return acore::INVALID_INDEX;
    }

private:
    std::unordered_map<acore::size_type, std::vector<acore::DataIndexMapElement>> mData;
};

class MapMultiHash
{
public:
    explicit MapMultiHash(const std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> &data)
    {
        for (auto val : data)
        {
            mData.insert(val);
        }
    }

    [[nodiscard]] auto value(acore::size_type element, acore::size_type key) const
    {
        const auto range = mData.equal_range(element);

        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second.key == key)
            {
                return it->second.value;
            }
        }

        return acore::INVALID_INDEX;
    }

private:
    std::unordered_multimap<acore::size_type, acore::DataIndexMapElement> mData;
};

TEST_CASE("DataIndexMap")
{
    int dataIndexMap = 0;
    int mapHashVector = 0;
    int mapVectorLinkedList = 0;
    int map2DVector = 0;
    int mapHashBuckets = 0;
    int mapMultiHash = 0;

    constexpr int elements = 1000;
    constexpr int keys = 100;

    std::vector<std::pair<acore::size_type, acore::DataIndexMapElement>> data;
    data.reserve(elements * keys);

    for (int k = 0; k < keys; ++k)
    {
        for (int e = 0; e < elements; ++e)
        {
            data.emplace_back(std::pair<acore::size_type, acore::DataIndexMapElement>{e, {k, e * k}});
        }
    }

    BENCHMARK_ADVANCED("[acore::DataIndexMap]")
    (Catch::Benchmark::Chronometer meter)
    {
        dataIndexMap = meter.runs();

        acore::DataIndexMap map{data};

        meter.measure([&] {
            const acore::size_type val = map.value(elements / 2, keys / 2);
            return val;
        });
    };

    BENCHMARK_ADVANCED("[2d vector]")
    (Catch::Benchmark::Chronometer meter)
    {
        map2DVector = meter.runs();

        Map2DVector map{data};

        meter.measure([&] {
            const acore::size_type val = map.value(elements / 2, keys / 2);
            return val;
        });
    };

    BENCHMARK_ADVANCED("[hash + buckets]")
    (Catch::Benchmark::Chronometer meter)
    {
        mapHashBuckets = meter.runs();

        MapHashBuckets map{data};

        meter.measure([&] {
            const acore::size_type val = map.value(elements / 2, keys / 2);
            return val;
        });
    };

    BENCHMARK_ADVANCED("[vector linked list]")
    (Catch::Benchmark::Chronometer meter)
    {
        mapVectorLinkedList = meter.runs();

        MapVectorLinkedList map{data};

        meter.measure([&] {
            const acore::size_type val = map.value(elements / 2, keys / 2);
            return val;
        });
    };

    BENCHMARK_ADVANCED("[hash index + vector linked list]")
    (Catch::Benchmark::Chronometer meter)
    {
        mapHashVector = meter.runs();

        MapHashVector map{data};

        meter.measure([&] {
            const acore::size_type val = map.value(elements / 2, keys / 2);
            return val;
        });
    };

    BENCHMARK_ADVANCED("[multihash]")
    (Catch::Benchmark::Chronometer meter)
    {
        mapMultiHash = meter.runs();

        MapMultiHash map{data};

        meter.measure([&] {
            const acore::size_type val = map.value(elements / 2, keys / 2);
            return val;
        });
    };

    // 2D Vector is the current imeplementation of DataIndexMap and is therefore excluded from benchmark requierement.
    //REQUIRE(dataIndexMap > map2DVector);
    REQUIRE(dataIndexMap > mapHashBuckets);
    REQUIRE(dataIndexMap > mapVectorLinkedList);
    REQUIRE(dataIndexMap > mapHashVector);
    REQUIRE(dataIndexMap > mapMultiHash);
}
}
