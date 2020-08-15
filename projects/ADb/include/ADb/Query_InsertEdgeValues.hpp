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

#ifndef ADB_QUERY_INSERTEDGEVALUES_HPP
#define ADB_QUERY_INSERTEDGEVALUES_HPP

#include "Query.hpp"

namespace adb
{
//! The Query::InsertEdgeValues class is part of
//! the builder of the adb::Query.
class Query::InsertEdgeValues : public Query::Id
{
public:
    using Query::Id::Id;

    //! Sets the values to be inserted to the edge
    //! to \a values.
    [[nodiscard]] auto values(std::vector<adb::KeyValue> values) && -> Query::Id;

    //! Sets the values to be inserted to the edge
    //! to \a placeholder to be bound later with
    //! single values set.
    [[nodiscard]] auto values(const Placeholder::Values &placeholder) && -> Query::Id;

    //! Sets the values to be inserted to the edge
    //! to the result of the \a subQuery.
    [[nodiscard]] auto values(Query::Values subQuery) && -> Query::Id;
};
}

#endif
