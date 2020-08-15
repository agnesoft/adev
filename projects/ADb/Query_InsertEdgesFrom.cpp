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

#include "Query_InsertEdgesFrom.hpp"
#include "Query_InsertEdgesTo.hpp"
#include "Query_InsertEdgesToEach.hpp"

namespace adb
{
auto Query::InsertEdgesFrom::from(std::vector<acore::size_type> ids) && -> Query::InsertEdgesTo
{
    std::get<InsertEdgeData>(mQuery.mData).from = std::move(ids);
    return Query::InsertEdgesTo{std::move(mQuery)};
}

auto Query::InsertEdgesFrom::from(const Placeholder::Ids &placeholder) && -> Query::InsertEdgesTo
{
    mQuery.addPlaceholder(placeholder.name, bindInsertEdgesFrom);
    return Query::InsertEdgesTo{std::move(mQuery)};
}

auto Query::InsertEdgesFrom::from(Query::Ids subQuery) && -> Query::InsertEdgesTo
{
    mQuery.addSubQuery(std::move(subQuery), bindInsertEdgesFrom);
    return Query::InsertEdgesTo{std::move(mQuery)};
}

auto Query::InsertEdgesFrom::fromEach(std::vector<acore::size_type> ids) && -> Query::InsertEdgesToEach
{
    auto &data = std::get<InsertEdgeData>(mQuery.mData);
    data.type = InsertEdgeData::Type::Assymmetric;
    data.from = std::move(ids);
    return Query::InsertEdgesToEach{std::move(mQuery)};
}

auto Query::InsertEdgesFrom::fromEach(const Placeholder::Ids &placeholder) && -> Query::InsertEdgesToEach
{
    std::get<InsertEdgeData>(mQuery.mData).type = InsertEdgeData::Type::Assymmetric;
    mQuery.addPlaceholder(placeholder.name, bindInsertEdgesFrom);
    return Query::InsertEdgesToEach{std::move(mQuery)};
}

auto Query::InsertEdgesFrom::fromEach(Query::Ids subQuery) && -> Query::InsertEdgesToEach
{
    std::get<InsertEdgeData>(mQuery.mData).type = InsertEdgeData::Type::Assymmetric;
    mQuery.addSubQuery(std::move(subQuery), bindInsertEdgesFrom);
    return Query::InsertEdgesToEach{std::move(mQuery)};
}
}
