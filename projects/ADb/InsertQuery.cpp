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

#include "InsertEdgeQuery.hpp"
#include "InsertQuery.hpp"
#include "Query.hpp"

#include <string>
#include <vector>

namespace adb
{
template<typename Data>
auto InsertQuery::createPlaceholderQuery(std::string name, BindPlaceholderFunction bind) -> IdsQuery
{
    Query query{Data{}};
    query.addPlaceholder(std::move(name), bind);
    return IdsQuery{std::move(query)};
}

template<typename Data>
auto InsertQuery::createSubQueryQuery(Query &&subQuery, BindResultFunction bind) -> IdsQuery
{
    Query query{Data{}};
    query.addSubQuery(std::move(subQuery), bind);
    return IdsQuery{std::move(query)};
}

auto InsertQuery::edge() && -> InsertEdgeQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return InsertEdgeQuery{};
}

auto InsertQuery::node() && -> IdsQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdsQuery{Query{InsertNodesCount{1}}};
}

auto InsertQuery::node(std::vector<KeyValue> values) && -> IdsQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdsQuery{Query{InsertNodeValues{std::move(values)}}};
}

auto InsertQuery::node(const PlaceholderValues &placeholder) && -> IdsQuery
{
    return createPlaceholderQuery<InsertNodeValues>(placeholder.name, bindInsertNodeValues);
}

auto InsertQuery::node(ValuesQuery subQuery) && -> IdsQuery
{
    return createSubQueryQuery<InsertNodeValues>(std::move(subQuery), bindInsertNodeValues);
}

auto InsertQuery::nodes(acore::size_type count) && -> IdsQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdsQuery{Query{InsertNodesCount{count}}};
}

auto InsertQuery::nodes(const PlaceholderCount &placeholder) && -> IdsQuery
{
    return createPlaceholderQuery<InsertNodesCount>(placeholder.name, bindInsertNodesCount);
}

auto InsertQuery::nodes(CountQuery subQuery) && -> IdsQuery
{
    return createSubQueryQuery<InsertNodesCount>(std::move(subQuery), bindInsertNodesCount);
}

auto InsertQuery::nodes(std::vector<std::vector<KeyValue>> values) && -> IdsQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdsQuery{Query{InsertNodesValues{std::move(values)}}};
}

auto InsertQuery::nodes(const PlaceholderValues &placeholder) && -> IdsQuery
{
    return createPlaceholderQuery<InsertNodesValues>(placeholder.name, bindInsertNodesValues);
}

auto InsertQuery::nodes(ValuesQuery subQuery) && -> IdsQuery
{
    return createSubQueryQuery<InsertNodesValues>(std::move(subQuery), bindInsertNodesValues);
}
}
