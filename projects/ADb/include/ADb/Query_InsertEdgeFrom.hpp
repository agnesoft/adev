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

#ifndef ADB_QUERY_INSERTEDGEFROM_HPP
#define ADB_QUERY_INSERTEDGEFROM_HPP

#include "Query.hpp"

namespace adb
{
class Query::InsertEdgeFrom : public Query::Wrapper
{
public:
    using Query::Wrapper::Wrapper;

    [[nodiscard]] auto from(acore::size_type id) && -> Query::InsertEdgeTo;
    [[nodiscard]] auto from(const Placeholder::Id &placeholder) && -> Query::InsertEdgeTo;
    [[nodiscard]] auto from(Query::Id subQuery) && -> Query::InsertEdgeTo;
};
}

#endif
