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
#include "Query_Insert.hpp"
#include "Query_Select.hpp"

namespace adb
{
auto insert_into() -> Query::Insert
{
    return Query::Insert{};
}

auto select() -> Query::Select
{
    return Query::Select{};
}

auto Query::bind(std::string_view name, PlaceholderValue value) & -> void //NOLINT(performance-unnecessary-value-param)
{
    const auto placeholder = findPlaceholder(name);

    if (placeholder.bind == nullptr)
    {
        throw acore::Exception{} << "Placeholder '" << name << "' not found.";
    }

    placeholder.bind(std::move(value), placeholder.queryData);
}

auto Query::addPlaceholder(std::string placeholder, BindPlaceholderFunction bindFunction) -> void
{
    validatePlaceholder(placeholder);
    mPlaceholders.emplace_back(PlaceholderData{std::move(placeholder), bindFunction});
}

auto Query::addSubQuery(Query &&query, BindResultFunction bindFunction) -> void
{
    for (const auto &placeholder : query.mPlaceholders)
    {
        validatePlaceholder(placeholder.name);
    }

    mSubQueries.emplace_back(SubQuery{std::move(query), bindFunction});
}

auto Query::findPlaceholder(std::string_view name) -> PlaceholderBind
{
    for (const auto &subQuery : mSubQueries)
    {
        const auto placeholder = subQuery.query()->findPlaceholder(name);

        if (placeholder.bind != nullptr)
        {
            return placeholder;
        }
    }

    for (const auto &placeholder : mPlaceholders)
    {
        if (placeholder.name == name)
        {
            return {placeholder.bind, &mData};
        }
    }

    return {};
}

auto Query::validatePlaceholder(std::string_view name) const -> void
{
    for (const auto &subQuery : mSubQueries)
    {
        subQuery.query()->validatePlaceholder(name);
    }

    for (const auto &placeholder : mPlaceholders)
    {
        if (placeholder.name == name)
        {
            throw acore::Exception{} << "Placeholder '" << name << "' already exists.";
        }
    }
}
}
