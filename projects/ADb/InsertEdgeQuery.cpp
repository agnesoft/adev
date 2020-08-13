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

#include "InsertEdgeFromQuery.hpp"
#include "InsertEdgeQuery.hpp"

namespace adb
{
auto InsertEdgeQuery::from(acore::size_type id) && -> InsertEdgeFromQuery
{
    std::get<InsertEdgeData>(mQuery.mData).from.push_back(id);
    return InsertEdgeFromQuery{std::move(mQuery)};
}

auto InsertEdgeQuery::from(const PlaceholderId &placeholder) && -> InsertEdgeFromQuery
{
    mQuery.addPlaceholder(placeholder.name, bindInsertEdgeFrom);
    return InsertEdgeFromQuery{std::move(mQuery)};
}

auto InsertEdgeQuery::from(IdQuery subQuery) && -> InsertEdgeFromQuery
{
    mQuery.addSubQuery(std::move(subQuery), bindInsertEdgeFrom);
    return InsertEdgeFromQuery{std::move(mQuery)};
}
}
