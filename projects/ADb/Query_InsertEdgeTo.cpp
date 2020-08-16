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

#include "Query_InsertEdgeTo.hpp"
#include "Query_InsertEdgeValues.hpp"

namespace adb
{
Query::InsertEdgeTo::InsertEdgeTo(Query &&query) :
    mQuery{std::move(query)}
{
}

auto Query::InsertEdgeTo::to(acore::size_type id) && -> Query::InsertEdgeValues
{
    std::get<InsertEdgeData>(mQuery.mData).to.resize(1, id);
    return Query::InsertEdgeValues{std::move(mQuery)};
}

auto Query::InsertEdgeTo::to(const Placeholder::Id &placeholder) && -> Query::InsertEdgeValues
{
    mQuery.addPlaceholder(placeholder.name, bindInsertEdgeTo);
    return Query::InsertEdgeValues{std::move(mQuery)};
}

auto Query::InsertEdgeTo::to(Query::Id subQuery) && -> Query::InsertEdgeValues
{
    mQuery.addSubQuery(std::move(subQuery), bindInsertEdgeTo);
    return Query::InsertEdgeValues{std::move(mQuery)};
}
}
