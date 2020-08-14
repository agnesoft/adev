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

#include "Query_Insert.hpp"
#include "Query_InsertEdge.hpp"

namespace adb
{
auto Query::Insert::edge() && -> Query::InsertEdge //NOLINT(readability-convert-member-functions-to-static)
{
    return InsertEdge{Query{InsertEdgeData{}}};
}

auto Query::Insert::edge(std::vector<KeyValue> values) && -> Query::InsertEdge //NOLINT(readability-convert-member-functions-to-static)
{
    return InsertEdge{Query{InsertEdgeData{{std::move(values)}, {}, {}}}};
}

auto Query::Insert::edge(const PlaceholderValues &placeholder) && -> Query::InsertEdge //NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertEdgeData{}};
    query.addPlaceholder(placeholder.name, BindInsertEdgeValues);
    return Query::InsertEdge{std::move(query)};
}

auto Query::Insert::edge(ValuesQuery subQuery) && -> Query::InsertEdge //NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertEdgeData{}};
    query.addSubQuery(std::move(subQuery), BindInsertEdgeValues);
    return Query::InsertEdge{std::move(query)};
}

auto Query::Insert::node() && -> IdQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdQuery{Query{InsertNodeData{{{}}}}};
}

auto Query::Insert::node(std::vector<KeyValue> values) && -> IdQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdQuery{Query{InsertNodeData{{std::move(values)}}}};
}

auto Query::Insert::node(const PlaceholderValues &placeholder) && -> IdQuery
{
    return IdQuery{createInsertNodeQuery(placeholder.name, bindInsertNodeValues)};
}

auto Query::Insert::node(ValuesQuery subQuery) && -> IdQuery
{
    return IdQuery{createInsertNodeQuery(std::move(subQuery), bindInsertNodeValues)};
}

auto Query::Insert::nodes(acore::size_type count) && -> IdsQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdsQuery{Query{InsertNodeData{std::vector<std::vector<adb::KeyValue>>(count)}}};
}

auto Query::Insert::nodes(const PlaceholderCount &placeholder) && -> IdsQuery
{
    return IdsQuery{createInsertNodeQuery(placeholder.name, bindInsertNodesCount)};
}

auto Query::Insert::nodes(CountQuery subQuery) && -> IdsQuery
{
    return IdsQuery{createInsertNodeQuery(std::move(subQuery), bindInsertNodesCount)};
}

auto Query::Insert::nodes(std::vector<std::vector<KeyValue>> values) && -> IdsQuery //NOLINT(readability-convert-member-functions-to-static)
{
    return IdsQuery{Query{InsertNodeData{std::move(values)}}};
}

auto Query::Insert::nodes(const PlaceholderMultiValues &placeholder) && -> IdsQuery
{
    return IdsQuery{createInsertNodeQuery(placeholder.name, bindInsertNodesValues)};
}

auto Query::Insert::nodes(MultiValuesQuery subQuery) && -> IdsQuery
{
    return IdsQuery{createInsertNodeQuery(std::move(subQuery), bindInsertNodesValues)};
}

auto Query::Insert::createInsertNodeQuery(std::string name, BindPlaceholderFunction bind) -> Query //NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertNodeData{}};
    query.addPlaceholder(std::move(name), bind);
    return query;
}

auto Query::Insert::createInsertNodeQuery(Query &&subQuery, BindResultFunction bind) -> Query //NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertNodeData{}};
    query.addSubQuery(std::move(subQuery), bind);
    return query;
}
}
