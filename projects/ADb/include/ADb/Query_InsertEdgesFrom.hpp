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

#ifndef ADB_QUERY_INSERTEDGESFROM_HPP
#define ADB_QUERY_INSERTEDGESFROM_HPP

#include "Query.hpp"

namespace adb
{
//! The Query::InsertEdgesFrom class is part of
//! the builder of the adb::Query.
class Query::InsertEdgesFrom
{
public:
    explicit InsertEdgesFrom(Query &&query);

    //! Sets the query's origin nodes to \a ids.
    //! The edges will be inserted symmetrically
    //! to \c to (destination) ids. There must be
    //! exactly the same number of \c from and \c to
    //! ids.
    [[nodiscard]] auto from(std::vector<acore::size_type> ids) && -> Query::InsertEdgesTo;

    //! Sets the query's origin nodes to a named
    //! \a placeholder to be bound later with ids.
    //! The edges will be inserted symmetrically
    //! to \c to (destination) ids. There must be
    //! exactly the same number of \c from and \c to
    //! ids.
    [[nodiscard]] auto from(const Placeholder::Ids &placeholder) && -> Query::InsertEdgesTo;

    //! Sets the query's origin nodes to a
    //! result of the \a subQuery. The edges will
    //! be inserted symmetrically to \c to
    //! (destination) ids. There must be
    //! exactly the same number of \c from and \c to
    //! ids.
    [[nodiscard]] auto from(Query::Ids subQuery) && -> Query::InsertEdgesTo;

    //! Sets the query's origin nodes to \a ids.
    //! The edges will be inserted from each of the
    //! \a ids to each of the \c to ids.
    [[nodiscard]] auto fromEach(std::vector<acore::size_type> ids) && -> Query::InsertEdgesToEach;

    //! Sets the query's origin nodes to a named
    //! \a placeholder to be bound later with ids.
    //! The edges wil be inserted from each of the
    //! bound ids to each of the \c to ids.
    [[nodiscard]] auto fromEach(const Placeholder::Ids &placeholder) && -> Query::InsertEdgesToEach;

    //! Sets the query's origin nodes to a named
    //! \a placeholder to be bound later with ids.
    //! The edges will be inserted from each of the
    //! \c from ids to each of the ids from the
    //! subquery's result.
    [[nodiscard]] auto fromEach(Query::Ids subQuery) && -> Query::InsertEdgesToEach;

private:
    Query mQuery;
};
}

#endif
