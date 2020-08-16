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

#ifndef ADB_QUERY_INSERTNODESVALUES_HPP
#define ADB_QUERY_INSERTNODESVALUES_HPP

#include "Query.hpp"

namespace adb
{
//! The Query::InsertNodesValues class is part of
//! the builder of the adb::Query.
class Query::InsertNodesValues
{
public:
    explicit InsertNodesValues(Query &&query);

    //! Sets the values to be inserted to the nodes
    //! to \a values. This overlod makes the values
    //! of each inserted node identical (equal to
    //! \a values).
    [[nodiscard]] auto values(std::vector<adb::KeyValue> values) && -> Query::Ids;

    //! Sets the values to be inserted to the nodes
    //! to \a placeholder to be bound later with single
    //! element's values. This overlod makes the values
    //! of each inserted node identical (equal to
    //! placeholder bound values).
    [[nodiscard]] auto values(const Placeholder::Values &placeholder) && -> Query::Ids;

    //! Sets the values to be inserted to the nodes
    //! to the result of the \a subQuery. This overload
    //! makes the values of each inserted node identical
    //! (equal to the result of the \a subQuery).
    [[nodiscard]] auto values(Query::Values subQuery) && -> Query::Ids;

    //! Sets the values to be inserted to the nodes
    //! to \a values. The number of values sets in
    //! \a values must be equal to the number of nodes
    //! being inserted.
    [[nodiscard]] auto values(std::vector<std::vector<adb::KeyValue>> values) && -> Query::Ids;

    //! Sets the values to be inserted to the nodes
    //! to \a placeholder to be bound later with multi-set
    //! of values. The number of values bound must
    //! be equal to the number of nodes being inserted.
    [[nodiscard]] auto values(const Placeholder::MultiValues &placeholder) && -> Query::Ids;

    //! Sets the values to be inserted to the nodes
    //! to the result of the \a subQuery. The result's
    //! number of values sets must be equal to the
    //! number of nodes being inserted.
    [[nodiscard]] auto values(Query::MultiValues subQuery) && -> Query::Ids;

private:
    Query mQuery;
};
}

#endif
