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

#ifndef ADB_QUERY_DATA_HPP
#define ADB_QUERY_DATA_HPP

#include "ADbModule.hpp"
#include "KeyValue.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace adb
{
class Query;

//! \cond IMPLEMENTAION_DETAIL
struct SubQuery
{
    SubQuery() = default;
    SubQuery(std::unique_ptr<Query> subQuery);
    SubQuery(const SubQuery &other);
    SubQuery(SubQuery &&other) noexcept = default;
    ~SubQuery();

    auto operator=(const SubQuery &other) -> SubQuery &;
    auto operator=(SubQuery &&other) noexcept -> SubQuery & = default;

    std::unique_ptr<Query> query;
};

struct InsertNodeQuery : SubQuery
{
};

struct InsertNodeValues
{
    std::vector<adb::KeyValue> values;
};

struct InsertNodesCount
{
    acore::size_type count = 0;
};

struct InsertNodesQuery : SubQuery
{
};

struct InsertNodesValues
{
    std::vector<std::vector<adb::KeyValue>> values;
};
//! \endcond

//! \relates adb::Query
//! Represents the possible internal data types
//! of the database query.
using QueryData = std::variant<std::monostate,
                               InsertNodeQuery,
                               InsertNodeValues,
                               InsertNodesCount,
                               InsertNodesQuery,
                               InsertNodesValues>;

//! \relates adb::Query
//! Types of values that can be bound to a query
//! data.
using PlaceholderValue = std::variant<acore::size_type,
                                      std::vector<acore::size_type>,
                                      std::vector<adb::KeyValue>,
                                      std::vector<std::vector<adb::KeyValue>>>;

//! \cond IMPLEMENTAION_DETAIL
struct Placeholder
{
    std::string name;
    auto (*bind)(PlaceholderValue &&value, QueryData *data) -> void = nullptr;
};
//! \endcond
}

#endif
