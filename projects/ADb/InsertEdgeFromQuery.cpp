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

namespace adb
{
auto InsertEdgeFromQuery::to(acore::size_type id) && -> IdsQuery
{
    std::get<InsertEdgesCount>(mQuery.mData).to = id;
    return IdsQuery{std::move(mQuery)};
}

auto InsertEdgeFromQuery::to(const PlaceholderId &placeholder) && -> IdsQuery
{
    mQuery.addPlaceholder(placeholder.name, bindInsertEdgesCountTo);
    return IdsQuery{std::move(mQuery)};
}

auto InsertEdgeFromQuery::to(IdsQuery subQuery) && -> IdsQuery
{
    mQuery.addSubQuery(std::move(subQuery), bindInsertEdgesCountTo);
    return IdsQuery{std::move(mQuery)};
}
}
