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

#ifndef ADB_INSERTQUERY_HPP
#define ADB_INSERTQUERY_HPP

#include "ADbModule.hpp"
#include "KeyValue.hpp"
#include "Placeholders.hpp"
#include "QueryData.hpp"
#include "SubQuery.hpp"

#include <string>
#include <vector>

namespace adb
{
class Query;
class IdsQuery;
class ValuesQuery;
class CountQuery;

class InsertQuery
{
public:
    [[nodiscard]] auto node() && -> IdsQuery;
    [[nodiscard]] auto node(std::vector<adb::KeyValue> values) && -> IdsQuery;
    [[nodiscard]] auto node(PlaceholderValues placeholder) && -> IdsQuery;
    [[nodiscard]] auto node(ValuesQuery subQuery) && -> IdsQuery;
    [[nodiscard]] auto nodes(acore::size_type count) && -> IdsQuery;
    [[nodiscard]] auto nodes(PlaceholderCount placeholder) && -> IdsQuery;
    [[nodiscard]] auto nodes(std::vector<std::vector<adb::KeyValue>> values) && -> IdsQuery;
    [[nodiscard]] auto nodes(PlaceholderValues placeholder) && -> IdsQuery;
    [[nodiscard]] auto nodes(CountQuery subQuery) && -> IdsQuery;
    [[nodiscard]] auto nodes(ValuesQuery subQuery) && -> IdsQuery;

private:
    template<typename Data>
    [[nodiscard]] auto createPlaceholderQuery(std::string name, BindPlaceholderFunction bind) -> IdsQuery;
    template<typename Data>
    [[nodiscard]] auto createSubQueryQuery(Query &&subQuery, BindResultFunction bind) -> IdsQuery;
};
}

#endif
