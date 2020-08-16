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

#include "Query_InsertEdgeFrom.hpp"
#include "Query_InsertEdgeTo.hpp"

namespace adb
{
Query::InsertEdgeFrom::InsertEdgeFrom(Query &&query) :
    mQuery{std::move(query)}
{
}

auto Query::InsertEdgeFrom::from(acore::size_type id) && -> Query::InsertEdgeTo
{
    std::get<InsertEdgeData>(mQuery.mData).from.resize(1, id);
    return Query::InsertEdgeTo{std::move(mQuery)};
}

auto Query::InsertEdgeFrom::from(const Placeholder::Id &placeholder) && -> Query::InsertEdgeTo
{
    mQuery.addPlaceholder(placeholder.name, bindInsertEdgeFrom);
    return Query::InsertEdgeTo{std::move(mQuery)};
}

auto Query::InsertEdgeFrom::from(Query::Id subQuery) && -> Query::InsertEdgeTo
{
    mQuery.addSubQuery(std::move(subQuery), bindInsertEdgeFrom);
    return Query::InsertEdgeTo{std::move(mQuery)};
}
}
