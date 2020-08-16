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

#ifndef ADB_QUERY_INSERTEDGESTOEACH_HPP
#define ADB_QUERY_INSERTEDGESTOEACH_HPP

#include "Query.hpp"

namespace adb
{
//! The Query::InsertEdgesToEach class is part of
//! the builder of the adb::Query.
class Query::InsertEdgesToEach
{
public:
    explicit InsertEdgesToEach(Query &&query);

    //! Sets the query's destination nodes to \a ids.
    //! The edges will be inserted from each of the
    //! \c from ids to each of the \a ids.
    [[nodiscard]] auto toEach(std::vector<acore::size_type> ids) && -> Query::InsertEdgesValues;

    //! Sets the query's destination nodes to a named
    //! \a placeholder to be bound later with ids.
    //! The edges wil be inserted from each of the
    //! \c from ids to each of the bound ids.
    [[nodiscard]] auto toEach(const Placeholder::Ids &placeholder) && -> Query::InsertEdgesValues;

    //! Sets the query's destination nodes to a
    //! result of the \a subQuery. The edges will
    //! be inserted from each of the \c from ids to
    //! each of the ids from the subquery's result.
    [[nodiscard]] auto toEach(Query::Ids subQuery) && -> Query::InsertEdgesValues;

private:
    Query mQuery;
};
}

#endif
