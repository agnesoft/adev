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

#ifndef ADB_INSERTEDGEQUERY_HPP
#define ADB_INSERTEDGEQUERY_HPP

#include "ADbModule.hpp"
#include "Placeholders.hpp"
#include "SubQuery.hpp"

namespace adb
{
class IdsQuery;
class InsertEdgeFromQuery;

//! The InsertEdgeQuery class is a helper class
//! that facilitates creating a query that insert
//! edges.
class InsertEdgeQuery
{
public:
    //! Sets the \c from parameter to \a id in the
    //! query. The \a id must be a valid node in
    //! the database.
    [[nodiscard]] auto from(acore::size_type id) && -> InsertEdgeFromQuery;

    //! Sets the \c from parameter of the query to
    //! a \a placeholder to be bound later.
    [[nodiscard]] auto from(PlaceholderId placeholder) && -> InsertEdgeFromQuery;

    //! Sets the \c from parameter of the query to
    //! the \a subQuery. The \c from parameter will
    //! come from the result of the \a subQuery.
    [[nodiscard]] auto from(IdsQuery subQuery) && -> InsertEdgeFromQuery;
};
}

#endif
