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

#ifndef ADB_QUERY_INSERT_HPP
#define ADB_QUERY_INSERT_HPP

#include "Query.hpp"

namespace adb
{
class Query::Insert
{
public:
    [[nodiscard]] auto edge() && -> Query::InsertEdgeFrom;
    [[nodiscard]] auto edges() && -> Query::InsertEdgesFrom;
    [[nodiscard]] auto edges(acore::size_type count) && -> Query::InsertEdgesFrom;
    [[nodiscard]] auto edges(const Placeholder::Count &placeholder) && -> Query::InsertEdgesFrom;
    [[nodiscard]] auto edges(Query::Count subQuery) && -> Query::InsertEdgesFrom;
    [[nodiscard]] auto node() && -> Query::InsertNodesValuesOptional;
    [[nodiscard]] auto nodes() && -> Query::InsertNodesValues;
    [[nodiscard]] auto nodes(acore::size_type count) && -> Query::InsertNodesValuesOptional;
    [[nodiscard]] auto nodes(const Placeholder::Count &placeholder) && -> Query::InsertNodesValuesOptional;
    [[nodiscard]] auto nodes(Query::Count subQuery) && -> Query::InsertNodesValuesOptional;
};
}

#endif
