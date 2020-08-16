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

#ifndef ADB_QUERY_INSERTEDGESTO_HPP
#define ADB_QUERY_INSERTEDGESTO_HPP

#include "Query.hpp"

namespace adb
{
//! The Query::InsertEdgesTo class is part of
//! the builder of the adb::Query.
class Query::InsertEdgesTo
{
public:
    //! Constructs the builder by moving from \a query.
    explicit InsertEdgesTo(Query &&query);

    //! Sets the query's destination nodes to \a ids.
    //! The edges will be inserted symmetrically to
    //! \c from ids. There must be exactly the same
    //! number of \c from and \c to ids.
    [[nodiscard]] auto to(std::vector<acore::size_type> ids) && -> Query::InsertEdgesValues;

    //! Sets the query's destination nodes to a named
    //! \a placeholder to be bound later with ids.
    //! The edges will be inserted symmetrically to
    //! \c from ids. There must be exactly the same
    //! number of \c from and \c to ids.
    [[nodiscard]] auto to(const Placeholder::Ids &placeholder) && -> Query::InsertEdgesValues;

    //! Sets the query's destination nodes to a
    //! result of the \a subQuery. The edges will
    //! be inserted   symmetrically to \c from ids.
    //! There must be exactly the same number of
    //! \c from and \c to ids.
    [[nodiscard]] auto to(Query::Ids subQuery) && -> Query::InsertEdgesValues;

private:
    Query mQuery;
};
}

#endif
