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

#include "Query_InsertEdgesValues.hpp"

namespace adb
{
auto Query::InsertEdgesValues::values(std::vector<KeyValue> values) && -> Query::Ids
{
    std::get<InsertEdgeData>(mData).values = std::vector<std::vector<adb::KeyValue>>{std::move(values)};
    return Query::Ids{std::move(*this)};
}

auto Query::InsertEdgesValues::values(const Placeholder::Values &placeholder) && -> Query::Ids
{
    addPlaceholder(placeholder.name, bindInsertEdgeValues);
    return Query::Ids{std::move(*this)};
}

auto Query::InsertEdgesValues::values(Query::Values subQuery) && -> Query::Ids
{
    addSubQuery(std::move(subQuery), bindInsertEdgeValues);
    return Query::Ids{std::move(*this)};
}

auto Query::InsertEdgesValues::values(std::vector<std::vector<KeyValue>> values) && -> Query::Ids
{
    std::get<InsertEdgeData>(mData).values = std::move(values);
    return Query::Ids{std::move(*this)};
}

auto Query::InsertEdgesValues::values(const Placeholder::MultiValues &placeholder) && -> Query::Ids
{
    addPlaceholder(placeholder.name, bindInsertEdgesValues);
    return Query::Ids{std::move(*this)};
}

auto Query::InsertEdgesValues::values(Query::MultiValues subQuery) && -> Query::Ids
{
    addSubQuery(std::move(subQuery), bindInsertEdgesValues);
    return Query::Ids{std::move(*this)};
}
}
