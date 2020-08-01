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

#ifndef ADB_CONDITION_HPP
#define ADB_CONDITION_HPP

#include "ADbModule.hpp"
#include "Value.hpp"

#include <utility>
#include <variant>
#include <vector>

namespace adb
{
//! \cond IMPLEMENTAION_DETAIL
class Condition
{
public:
    using ValueComparisonOperator = auto (*)(const Value &left, const Value &right) -> bool;
    using CountComparisonOperator = auto (*)(acore::size_type left, acore::size_type right) -> bool;

    struct Count
    {
        acore::size_type count = 0;

        CountComparisonOperator compare = [](acore::size_type left, acore::size_type right) {
            return left == right;
        };
    };

    struct Id
    {
        acore::size_type id = acore::INVALID_INDEX;
    };

    struct Ids
    {

        std::vector<acore::size_type> ids;
    };

    struct And
    {
    };

    struct BeyondId : Id
    {
    };

    struct BeyondIds : Ids
    {
    };

    struct Distance : Count
    {
    };

    struct Edge
    {
    };

    struct EdgeCount : Count
    {
    };

    struct EndWhere
    {
    };

    struct FromCount : Count
    {
    };

    struct Key
    {
        adb::Value key;
        ValueComparisonOperator keyCompare = [](const adb::Value &left, const adb::Value &right) {
            return left == right;
        };
    };

    struct Or
    {
    };

    struct Node
    {
    };

    struct Not
    {
    };

    struct ToCount : Count
    {
        using Count::Count;
    };

    struct Where
    {
    };

    struct Value : Key
    {

        adb::Value value;
        ValueComparisonOperator valueCompare = [](const adb::Value &left, const adb::Value &right) {
            return left == right;
        };
    };

    using Data = std::variant<And,
                              BeyondId,
                              BeyondIds,
                              Distance,
                              Edge,
                              EdgeCount,
                              EndWhere,
                              FromCount,
                              Id,
                              Ids,
                              Key,
                              Node,
                              Not,
                              Or,
                              ToCount,
                              Value,
                              Where>;

    template<typename Data>
    explicit Condition(Data data) noexcept :
        mData{data}
    {
    }

    [[nodiscard]] auto data() const noexcept -> const Data &
    {
        return mData;
    }

    template<typename DataT>
    auto setData(DataT data) -> void
    {
        if (!std::holds_alternative<DataT>(mData))
        {
            const char *dataName = std::visit([](auto &&data) -> const char * {
                return typeid(data).name();
            },
                                              mData);
            throw acore::Exception{} << "Setting condition data of type '" << typeid(DataT).name() << "' to a condition of type '" << dataName;
        }

        mData = std::move(data);
    }

private:
    Data mData;
};
//! \endcond
}

#endif
