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

#ifndef ADB_QUERY_INSERTEDGE_HPP
#define ADB_QUERY_INSERTEDGE_HPP

#include "Query.hpp"

namespace adb
{
//! The InsertEdgeQuery class is a helper class
//! that facilitates creating a query that insert
//! edges.
class Query::InsertEdge : public Query::Base
{
public:
    using Query::Base::Base;

    //! Sets the \c from parameter to \a id in the
    //! query. The \a id must be a valid node in
    //! the database.
    [[nodiscard]] auto from(acore::size_type id) && -> InsertEdgeFrom;

    //! Sets the \c from parameter of the query to
    //! a \a placeholder to be bound later.
    [[nodiscard]] auto from(const PlaceholderId &placeholder) && -> InsertEdgeFrom;

    //! Sets the \c from parameter of the query to
    //! the \a subQuery. The \c from parameter will
    //! come from the result of the \a subQuery.
    [[nodiscard]] auto from(IdQuery subQuery) && -> InsertEdgeFrom;
};
}

#endif
