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

#include "Query_InsertNodesValues.hpp"

namespace adb
{
auto Query::InsertNodesValues::values(std::vector<KeyValue> values) && -> Query::Ids
{
    std::get<InsertNodeData>(mQuery.mData).values = std::vector<std::vector<adb::KeyValue>>{std::move(values)};
    return Query::Ids{std::move(mQuery)};
}

auto Query::InsertNodesValues::values(const Placeholder::Values &placeholder) && -> Query::Ids
{
    mQuery.addPlaceholder(placeholder.name, bindInsertNodeValues);
    return Query::Ids{std::move(mQuery)};
}

auto Query::InsertNodesValues::values(Query::Values subQuery) && -> Query::Ids
{
    mQuery.addSubQuery(std::move(subQuery), bindInsertNodeValues);
    return Query::Ids{std::move(mQuery)};
}

auto Query::InsertNodesValues::values(std::vector<std::vector<KeyValue>> values) && -> Query::Ids
{
    auto &data = std::get<InsertNodeData>(mQuery.mData);

    if (data.count == 0)
    {
        data.count = static_cast<acore::size_type>(values.size());
    }

    data.values = std::move(values);
    return Query::Ids{std::move(mQuery)};
}

auto Query::InsertNodesValues::values(const Placeholder::MultiValues &placeholder) && -> Query::Ids
{
    mQuery.addPlaceholder(placeholder.name, bindInsertNodesValues);
    return Query::Ids{std::move(mQuery)};
}

auto Query::InsertNodesValues::values(Query::MultiValues subQuery) && -> Query::Ids
{
    mQuery.addSubQuery(std::move(subQuery), bindInsertNodesValues);
    return Query::Ids{std::move(mQuery)};
}
}
