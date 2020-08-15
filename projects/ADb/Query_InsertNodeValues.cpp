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

#include "Query_InsertNodeValues.hpp"

namespace adb
{
auto Query::InsertNodeValues::values(std::vector<KeyValue> values) && -> Query::Id
{
    std::get<InsertNodeData>(mData).values = std::vector<std::vector<adb::KeyValue>>{std::move(values)};
    return Query::Id{std::move(*this)};
}

auto Query::InsertNodeValues::values(const Placeholder::Values &placeholder) && -> Query::Id
{
    addPlaceholder(placeholder.name, bindInsertNodeValues);
    return Query::Id{std::move(*this)};
}

auto Query::InsertNodeValues::values(Query::Values subQuery) && -> Query::Id
{
    addSubQuery(std::move(subQuery), bindInsertNodeValues);
    return Query::Id{std::move(*this)};
}
}
