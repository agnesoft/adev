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

#include "pch.hpp"

#include "Query.hpp"
#include "SubQuery.hpp"

namespace adb
{
SubQuery::~SubQuery() = default;

SubQuery::SubQuery(Query &&subQuery, BindResultFunction bindFunction) :
    mQuery{std::make_unique<Query>(std::move(subQuery))},
    mBind{bindFunction}
{
}

SubQuery::SubQuery(const SubQuery &other)
{
    *this = other;
}

auto SubQuery::operator=(const SubQuery &other) -> SubQuery &
{
    if (&other != this)
    {
        mQuery = std::make_unique<Query>(*other.mQuery);
        mBind = other.mBind;
    }

    return *this;
}

auto SubQuery::bind() const noexcept -> BindResultFunction
{
    return mBind;
}

auto SubQuery::query() const noexcept -> Query *
{
    return mQuery.get();
}
}
