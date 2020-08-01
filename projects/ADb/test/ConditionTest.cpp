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

#include <type_traits>
#include <utility>
#include <vector>

namespace adb
{
[[nodiscard]] auto operator==(const Condition::Data &left, const Condition::Data &right) -> bool
{
    return std::visit([&](auto &&leftData) -> bool {
        return std::visit([&](auto &&rightData) -> bool {
            using LeftT = std::decay_t<decltype(leftData)>;
            using RightT = std::decay_t<decltype(rightData)>;

            if constexpr (std::is_same_v<LeftT, RightT>)
            {
                if constexpr (std::is_same_v<LeftT, adb::Condition::Id> || std::is_base_of_v<adb::Condition::Id, LeftT>)
                {
                    return leftData.id == rightData.id;
                }
                else if constexpr (std::is_same_v<LeftT, adb::Condition::Ids> || std::is_base_of_v<adb::Condition::Ids, LeftT>)
                {
                    return leftData.ids == rightData.ids;
                }
                else if constexpr (std::is_same_v<LeftT, adb::Condition::Key>)
                {
                    return leftData.key == rightData.key && leftData.keyCompare == rightData.keyCompare;
                }
                else if constexpr (std::is_base_of_v<LeftT, adb::Condition::Count>)
                {
                    return leftData.count == rightData.count && leftData.compare == rightData.compare;
                }
                else if constexpr (std::is_same_v<LeftT, Value>)
                {
                    return leftData.key == rightData.key
                        && leftData.keyCompare == rightData.keyCompare
                        && leftData.value == rightData.value
                        && leftData.valueCompare == rightData.valueCompare;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                return false;
            }
        },
                          right);
    },
                      left);
}
}

namespace conditiontest
{
TEST_CASE("[adb::Condition]")
{
    REQUIRE_FALSE(std::is_default_constructible_v<adb::Condition>);
    REQUIRE(std::is_copy_constructible_v<adb::Condition>);
    REQUIRE(std::is_nothrow_move_constructible_v<adb::Condition>);
    REQUIRE(std::is_copy_assignable_v<adb::Condition>);
    REQUIRE(std::is_nothrow_move_assignable_v<adb::Condition>);
    REQUIRE(std::is_nothrow_destructible_v<adb::Condition>);
}

TEST_CASE("Condition(Data data) noexcept [adb::Condition]")
{
    REQUIRE(noexcept(adb::Condition{adb::Condition::And{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::BeyondId{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::BeyondIds{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::Distance{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::Edge{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::EdgeCount{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::EndWhere{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::FromCount{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::Id{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::Ids{}}));

    {
        adb::Condition::Key keyData;
        REQUIRE(noexcept(adb::Condition{std::move(keyData)})); //NOLINT(hicpp-invalid-access-moved, bugprone-use-after-move)
    }

    REQUIRE(noexcept(adb::Condition{adb::Condition::Node{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::Not{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::Or{}}));
    REQUIRE(noexcept(adb::Condition{adb::Condition::ToCount{}}));

    {
        adb::Condition::Value valueData;
        REQUIRE(noexcept(adb::Condition{std::move(valueData)})); //NOLINT(hicpp-invalid-access-moved, bugprone-use-after-move)
    }

    REQUIRE(noexcept(adb::Condition{adb::Condition::Where{}}));
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::And{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::BeyondId{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::BeyondIds{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Distance{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Edge{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::EdgeCount{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::EndWhere{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::FromCount{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Id{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Ids{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Key{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Node{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Not{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Or{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::ToCount{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Value{}});
    REQUIRE_NOTHROW(adb::Condition{adb::Condition::Where{}});
}

TEST_CASE("data() const noexcept -> cont ConditionData & [adb::Condition]")
{
    const adb::Condition::Edge edgeConditionData;
    const adb::Condition condition{edgeConditionData};
    REQUIRE(noexcept(condition.data()));
    REQUIRE(condition.data() == adb::Condition::Data{edgeConditionData});
}

TEST_CASE("setData(Data data) -> void [adb::Condition]")
{
    SECTION("[same type]")
    {
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::And{}}.setData(adb::Condition::And{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::BeyondId{}}.setData(adb::Condition::BeyondId{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::BeyondIds{}}.setData(adb::Condition::BeyondIds{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Distance{}}.setData(adb::Condition::Distance{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Edge{}}.setData(adb::Condition::Edge{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::EdgeCount{}}.setData(adb::Condition::EdgeCount{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::EndWhere{}}.setData(adb::Condition::EndWhere{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::FromCount{}}.setData(adb::Condition::FromCount{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Id{}}.setData(adb::Condition::Id{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Ids{}}.setData(adb::Condition::Ids{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Key{}}.setData(adb::Condition::Key{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Node{}}.setData(adb::Condition::Node{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Not{}}.setData(adb::Condition::Not{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Or{}}.setData(adb::Condition::Or{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::ToCount{}}.setData(adb::Condition::ToCount{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Value{}}.setData(adb::Condition::Value{}));
        REQUIRE_NOTHROW(adb::Condition{adb::Condition::Where{}}.setData(adb::Condition::Where{}));
    }

    SECTION("[different type]")
    {
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::And{}}.setData(adb::Condition::Where{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::BeyondId{}}.setData(adb::Condition::And{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::BeyondIds{}}.setData(adb::Condition::Distance{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Distance{}}.setData(adb::Condition::Edge{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Edge{}}.setData(adb::Condition::BeyondId{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::EdgeCount{}}.setData(adb::Condition::Node{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::EndWhere{}}.setData(adb::Condition::EdgeCount{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::FromCount{}}.setData(adb::Condition::EndWhere{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Id{}}.setData(adb::Condition::Ids{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Ids{}}.setData(adb::Condition::Key{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Key{}}.setData(adb::Condition::Not{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Node{}}.setData(adb::Condition::FromCount{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Not{}}.setData(adb::Condition::Or{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Or{}}.setData(adb::Condition::BeyondIds{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::ToCount{}}.setData(adb::Condition::Id{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Value{}}.setData(adb::Condition::ToCount{}), acore::Exception);
        REQUIRE_THROWS_AS(adb::Condition{adb::Condition::Where{}}.setData(adb::Condition::Value{}), acore::Exception);
    }

    SECTION("[different value]")
    {
        SECTION("[id]")
        {
            const adb::Condition::Id idConditionData{4};
            adb::Condition condition{adb::Condition::Id{}};
            condition.setData(idConditionData);
            REQUIRE(condition.data() == idConditionData);
        }

        SECTION("[ids]")
        {
            const adb::Condition::Ids idsConditionData{{1, 2, 3, 4}};
            adb::Condition condition{adb::Condition::Ids{}};
            condition.setData(idsConditionData);
            REQUIRE(condition.data() == idsConditionData);
        }

        SECTION("[edge count]")
        {
            const adb::Condition::EdgeCount edgeCountData{{3}};
            adb::Condition condition{adb::Condition::EdgeCount{}};
            condition.setData(edgeCountData);
            REQUIRE(condition.data() == edgeCountData);
            REQUIRE(edgeCountData.compare(edgeCountData.count, 3));
        }

        SECTION("[edge count with comparator]")
        {
            const auto countComparator = [](acore::size_type left, acore::size_type right) {
                return left < right;
            };
            const adb::Condition::EdgeCount edgeCountData{{3, countComparator}};
            adb::Condition condition{adb::Condition::EdgeCount{}};
            condition.setData(edgeCountData);
            REQUIRE(condition.data() == edgeCountData);
            REQUIRE(edgeCountData.compare(edgeCountData.count, 4));
        }

        SECTION("[key]")
        {
            const adb::Condition::Key keyData{adb::Value{"Hello, World!"}};
            adb::Condition condition{adb::Condition::Key{}};
            condition.setData(keyData);
            REQUIRE(condition.data() == keyData);
            REQUIRE(keyData.keyCompare(keyData.key, adb::Value{"Hello, World!"}));
        }

        SECTION("[key with comparator]")
        {
            const adb::Condition::Key keyData{adb::Value{"Hello, World!"}, [](const adb::Value &left, const adb::Value &right) {
                                                  return left != right;
                                              }};
            adb::Condition condition{adb::Condition::Key{}};
            condition.setData(keyData);
            REQUIRE(condition.data() == keyData);
            REQUIRE(keyData.keyCompare(keyData.key, adb::Value{4}));
        }

        SECTION("[value]")
        {
            const adb::Condition::Value valueData{{adb::Value{"Key"}}, adb::Value{"Value"}};
            adb::Condition condition{adb::Condition::Value{}};
            condition.setData(valueData);
            REQUIRE(condition.data() == valueData);
            REQUIRE(valueData.keyCompare(valueData.key, adb::Value{"Key"}));
            REQUIRE(valueData.valueCompare(valueData.value, adb::Value{"Value"}));
        }

        SECTION("[value with comparators]")
        {
            const auto keyComparator = [](const adb::Value &left, const adb::Value &right) {
                return left != right;
            };
            const auto valueComparator = [](const adb::Value &left, const adb::Value &right) {
                return left != right;
            };
            const adb::Condition::Value valueData{{adb::Value{"Key"}, keyComparator}, adb::Value{"Value"}, valueComparator};
            adb::Condition condition{adb::Condition::Value{}};
            condition.setData(valueData);
            REQUIRE(condition.data() == valueData);
            REQUIRE(valueData.keyCompare(valueData.key, adb::Value{"DifferentKey"}));
            REQUIRE(valueData.valueCompare(valueData.value, adb::Value{"DifferentValue"}));
        }
    }
}
}
