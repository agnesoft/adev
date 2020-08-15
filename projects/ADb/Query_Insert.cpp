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
#include "Query_InsertEdgeFrom.hpp"
#include "Query_InsertEdgesFrom.hpp"
#include "Query_InsertNodeValues.hpp"
#include "Query_InsertNodesValues.hpp"
#include "Query_InsertNodesValuesOptional.hpp"

namespace adb
{
auto Query::Insert::edge() && -> Query::InsertEdgeFrom // NOLINT(readability-convert-member-functions-to-static)
{
    return Query::InsertEdgeFrom{Query{InsertEdgeData{1, {}, {}, {}}}};
}

auto Query::Insert::edges() && -> Query::InsertEdgesFrom // NOLINT(readability-convert-member-functions-to-static)
{
    return Query::InsertEdgesFrom{Query{InsertEdgeData{0, {}, {}, {}}}};
}

auto Query::Insert::edges(acore::size_type count) && -> Query::InsertEdgesFrom // NOLINT(readability-convert-member-functions-to-static)
{
    return Query::InsertEdgesFrom{Query{InsertEdgeData{count, {}, {}, {}}}};
}

auto Query::Insert::edges(const Placeholder::Count &placeholder) && -> Query::InsertEdgesFrom // NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertEdgeData{}};
    query.addPlaceholder(placeholder.name, bindInsertEdgesCount);
    return Query::InsertEdgesFrom{std::move(query)};
}

auto Query::Insert::edges(Query::Count subQuery) && -> Query::InsertEdgesFrom // NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertEdgeData{}};
    query.addSubQuery(std::move(subQuery), bindInsertEdgesCount);
    return Query::InsertEdgesFrom{std::move(query)};
}

auto Query::Insert::node() && -> Query::InsertNodesValuesOptional // NOLINT(readability-convert-member-functions-to-static)
{
    return Query::InsertNodesValuesOptional{Query{InsertNodeData{1, {}}}};
}

auto Query::Insert::nodes() && -> InsertNodesValues // NOLINT(readability-convert-member-functions-to-static)
{
    return Query::InsertNodesValues{Query{InsertNodeData{}}};
}

auto Query::Insert::nodes(acore::size_type count) && -> Query::InsertNodesValuesOptional // NOLINT(readability-convert-member-functions-to-static)
{
    return Query::InsertNodesValuesOptional{Query{InsertNodeData{count, {}}}};
}

auto Query::Insert::nodes(const Placeholder::Count &placeholder) && -> Query::InsertNodesValuesOptional // NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertNodeData{}};
    query.addPlaceholder(placeholder.name, bindInsertNodesCount);
    return Query::InsertNodesValuesOptional{std::move(query)};
}

auto Query::Insert::nodes(Query::Count subQuery) && -> Query::InsertNodesValuesOptional // NOLINT(readability-convert-member-functions-to-static)
{
    Query query{InsertNodeData{}};
    query.addSubQuery(std::move(subQuery), bindInsertNodesCount);
    return Query::InsertNodesValuesOptional{std::move(query)};
}
}
