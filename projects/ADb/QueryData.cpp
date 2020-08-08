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
#include "QueryData.hpp"

namespace adb
{
SubQuery::~SubQuery() = default;

SubQuery::SubQuery(std::unique_ptr<Query> subQuery) :
    query{std::move(subQuery)}
{
}

SubQuery::SubQuery(const SubQuery &other)
{
    *this = other;
}

auto SubQuery::operator=(const SubQuery &other) -> SubQuery &
{
    query = std::make_unique<Query>(*other.query);
    return *this;
}

}
