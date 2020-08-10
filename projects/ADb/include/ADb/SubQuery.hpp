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

#ifndef ADB_SUBQUERY_HPP
#define ADB_SUBQUERY_HPP

#include "ADbModule.hpp"
#include "Placeholders.hpp"
#include "QueryData.hpp"

#include <memory>

namespace adb
{
//! \cond IMPLEMENTAION_DETAIL
class Query;

using BindResultFunction = auto (*)(PlaceholderValue &&value, QueryData *data) -> void;

struct SubQuery
{
    SubQuery() = default;
    SubQuery(Query &&subQuery, BindResultFunction bindFunction);
    SubQuery(const SubQuery &other);
    SubQuery(SubQuery &&other) noexcept = default;
    ~SubQuery();

    auto operator=(const SubQuery &other) -> SubQuery &;
    auto operator=(SubQuery &&other) noexcept -> SubQuery & = default;

    std::unique_ptr<Query> query;
    BindResultFunction bind;
};
//! \endcond
}
#endif
