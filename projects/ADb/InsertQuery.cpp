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
auto InsertQuery::edge() && -> InsertEdgeQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return InsertEdgeQuery{Query{InsertEdgeData{}}};
}

auto InsertQuery::edge(std::vector<KeyValue> values) && -> InsertEdgeQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return InsertEdgeQuery{Query{InsertEdgeData{{std::move(values)}, {}, {}}}};
}

auto InsertQuery::edge(const PlaceholderValues &placeholder) && -> InsertEdgeQuery //NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertEdgeData{}};
    query.addPlaceholder(placeholder.name, BindInsertEdgeValues);
    return InsertEdgeQuery{std::move(query)};
}

auto InsertQuery::edge(ValuesQuery subQuery) && -> InsertEdgeQuery //NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertEdgeData{}};
    query.addSubQuery(std::move(subQuery), BindInsertEdgeValues);
    return InsertEdgeQuery{std::move(query)};
}

auto InsertQuery::node() && -> IdQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdQuery{Query{InsertNodeData{{{}}}}};
}

auto InsertQuery::node(std::vector<KeyValue> values) && -> IdQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdQuery{Query{InsertNodeData{{std::move(values)}}}};
}

auto InsertQuery::node(const PlaceholderValues &placeholder) && -> IdQuery
{
    return IdQuery{createInsertNodeQuery(placeholder.name, bindInsertNodeValues)};
}

auto InsertQuery::node(ValuesQuery subQuery) && -> IdQuery
{
    return IdQuery{createInsertNodeQuery(std::move(subQuery), bindInsertNodeValues)};
}

auto InsertQuery::nodes(acore::size_type count) && -> IdsQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdsQuery{Query{InsertNodeData{std::vector<std::vector<adb::KeyValue>>(count)}}};
}

auto InsertQuery::nodes(const PlaceholderCount &placeholder) && -> IdsQuery
{
    return IdsQuery{createInsertNodeQuery(placeholder.name, bindInsertNodesCount)};
}

auto InsertQuery::nodes(CountQuery subQuery) && -> IdsQuery
{
    return IdsQuery{createInsertNodeQuery(std::move(subQuery), bindInsertNodesCount)};
}

auto InsertQuery::nodes(std::vector<std::vector<KeyValue>> values) && -> IdsQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdsQuery{Query{InsertNodeData{std::move(values)}}};
}

auto InsertQuery::nodes(const PlaceholderMultiValues &placeholder) && -> IdsQuery
{
    return IdsQuery{createInsertNodeQuery(placeholder.name, bindInsertNodesValues)};
}

auto InsertQuery::nodes(MultiValuesQuery subQuery) && -> IdsQuery
{
    return IdsQuery{createInsertNodeQuery(std::move(subQuery), bindInsertNodesValues)};
}

auto InsertQuery::createInsertNodeQuery(std::string name, BindPlaceholderFunction bind) -> Query //NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertNodeData{}};
    query.addPlaceholder(std::move(name), bind);
    return query;
}

auto InsertQuery::createInsertNodeQuery(Query &&subQuery, BindResultFunction bind) -> Query //NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertNodeData{}};
    query.addSubQuery(std::move(subQuery), bind);
    return query;
}
}
