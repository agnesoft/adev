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
    query.addPlaceholder(name, bind);
    return IdsQuery{std::move(query)};
}

template<typename Data>
auto InsertQuery::createSubQueryQuery(Query &&subQuery, BindResultFunction bind) -> IdsQuery
{
    Query query{Data{}};
    query.addSubQuery(std::move(subQuery), bind);
    return IdsQuery{std::move(query)};
}

auto InsertQuery::node() && -> IdsQuery
{
    return IdsQuery{Query{InsertNodesCount{1}}};
}

auto InsertQuery::node(std::vector<KeyValue> values) && -> IdsQuery
{
    return IdsQuery{Query{InsertNodeValues{std::move(values)}}};
}

auto InsertQuery::node(PlaceholderValues placeholder) && -> IdsQuery
{
    return createPlaceholderQuery<InsertNodeValues>(placeholder.name, [](PlaceholderValue &&value, QueryData *data) {
        std::get<InsertNodeValues>(*data).values = std::move(std::get<std::vector<adb::KeyValue>>(value));
    });
}

auto InsertQuery::node(ValuesQuery subQuery) && -> IdsQuery
{
    return createSubQueryQuery<InsertNodeValues>(std::move(subQuery), [](PlaceholderValue &&value, QueryData *data) {
        std::get<InsertNodeValues>(*data).values = std::move(std::get<std::vector<adb::KeyValue>>(value));
    });
}

auto InsertQuery::nodes(acore::size_type count) && -> IdsQuery
{
    return IdsQuery{Query{InsertNodesCount{count}}};
}

auto InsertQuery::nodes(PlaceholderCount placeholder) && -> IdsQuery
{
    return createPlaceholderQuery<InsertNodesCount>(placeholder.name, [](PlaceholderValue &&value, QueryData *data) {
        std::get<InsertNodesCount>(*data).count = std::get<acore::size_type>(value);
    });
}

auto InsertQuery::nodes(std::vector<std::vector<KeyValue>> values) && -> IdsQuery
{
    return IdsQuery{Query{InsertNodesValues{std::move(values)}}};
}

auto InsertQuery::nodes(PlaceholderValues placeholder) && -> IdsQuery
{
    return createPlaceholderQuery<InsertNodesValues>(placeholder.name, [](PlaceholderValue &&value, QueryData *data) {
        std::get<InsertNodesValues>(*data).values = std::move(std::get<std::vector<std::vector<adb::KeyValue>>>(value));
    });
}

auto InsertQuery::nodes(CountQuery subQuery) && -> IdsQuery
{
    return createSubQueryQuery<InsertNodesCount>(std::move(subQuery), [](PlaceholderValue &&value, QueryData *data) {
        std::get<InsertNodesCount>(*data).count = std::get<acore::size_type>(value);
    });
}

auto InsertQuery::nodes(ValuesQuery subQuery) && -> IdsQuery
{
    return createSubQueryQuery<InsertNodesValues>(std::move(subQuery), [](PlaceholderValue &&value, QueryData *data) {
        std::get<InsertNodesValues>(*data).values = std::move(std::get<std::vector<std::vector<adb::KeyValue>>>(value));
    });
}
}
