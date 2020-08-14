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

#include "Query_InsertEdge.hpp"
#include "Query_InsertEdgeFrom.hpp"

namespace adb
{
auto Query::InsertEdge::from(acore::size_type id) && -> InsertEdgeFrom
{
    std::get<InsertEdgeData>(mQuery.mData).from.push_back(id);
    return InsertEdgeFrom{std::move(mQuery)};
}

auto Query::InsertEdge::from(const PlaceholderId &placeholder) && -> InsertEdgeFrom
{
    mQuery.addPlaceholder(placeholder.name, bindInsertEdgeFrom);
    return InsertEdgeFrom{std::move(mQuery)};
}

auto Query::InsertEdge::from(IdQuery subQuery) && -> InsertEdgeFrom
{
    mQuery.addSubQuery(std::move(subQuery), bindInsertEdgeFrom);
    return InsertEdgeFrom{std::move(mQuery)};
}
}
