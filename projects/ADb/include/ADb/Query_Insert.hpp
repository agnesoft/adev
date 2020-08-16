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
//! The Query::Insert class is part of the builder
//! of the adb::Query.
class Query::Insert
{
public:
    //! Insert single edge query.
    [[nodiscard]] auto edge() && -> Query::InsertEdgeFrom;

    //! Insert multiple edges query.
    [[nodiscard]] auto edges() && -> Query::InsertEdgesFrom;

    //! Insert \a count edges query.
    [[nodiscard]] auto edges(acore::size_type count) && -> Query::InsertEdgesFrom;

    //! Insert multiple edges query with the count
    //! being bound to a \a placeholder.
    [[nodiscard]] auto edges(const Placeholder::Count &placeholder) && -> Query::InsertEdgesFrom;

    //! Insert multiple edges query with the count
    //! being bound to the result of the \a subQuery.
    [[nodiscard]] auto edges(Query::Count subQuery) && -> Query::InsertEdgesFrom;

    //! Insert single node query.
    [[nodiscard]] auto node() && -> Query::InsertNodeValues;

    //! Insert multiple nodes query.
    [[nodiscard]] auto nodes() && -> Query::InsertNodesValues;

    //! Insert \a count nodes query.
    [[nodiscard]] auto nodes(acore::size_type count) && -> Query::InsertNodesValuesOptional;

    //! Insert multiple nodes query with the count
    //! being bound to a \a placeholder.
    [[nodiscard]] auto nodes(const Placeholder::Count &placeholder) && -> Query::InsertNodesValuesOptional;

    //! Insert multiple nodes query with the count
    //! being bound to the result of the \a subQuery.
    [[nodiscard]] auto nodes(Query::Count subQuery) && -> Query::InsertNodesValuesOptional;
};
}

#endif
