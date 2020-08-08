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

#ifndef ADB_QUERY_HPP
#define ADB_QUERY_HPP

#include "ADbModule.hpp"
#include "Condition.hpp"
#include "KeyValue.hpp"
#include "QueryData.hpp"
#include "Value.hpp"

#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace adb
{
//! The Query class represents a query to the database.
//! Queries are constructed using fluent interface
//! in order to provide semantic correctness (not
//! necessarily data correctness) and readability
//! to writing queries (unlike plain text queries).
//!
//! To start constructing a query use one of the three
//! basic standalone functions in the namespace adb:
//!
//! - adb::insert()
//! - adb::remove()
//! - adb::select()
//!
//! When using IDE you will experience essentially
//! a guided construction of your query.
//!
//! The Query supports both nested queries and placeholder
//! values. Nested queries are passed in during the
//! construction of the query in place of values where
//! it makes sense (e.g. list of element ids, list
//! of values etc.). Placeholders can be used in
//! essentially the same places and must be later
//! bound using bind() method on the resulting query.
class Query
{
public:
    class Insert;

    //! Binds a value of a placehodler associated
    //! with \a name to \a value. Note that if the
    //! \a value 's type does not match the type
    //! expected by the placeholding value an exception
    //! is thrown.
    auto bind(const char *name, PlaceholderValue value) -> void
    {
        auto it = std::find_if(mPlaceholders.begin(), mPlaceholders.end(), [&](const Placeholder &placeholder) {
            return placeholder.name == name;
        });

        if (it == mPlaceholders.end())
        {
            throw acore::Exception{} << "Placeholder '" << name << "' not found.";
        }

        it->bind(std::move(value), &mData);
    }

    //! Returns the data of the query. Used by the
    //! database to access the query's data to execute.
    [[nodiscard]] auto data() const noexcept -> const QueryData &
    {
        return mData;
    }

    //! \relates adb::Query
    //! Stand alone function that begins composing
    //! the insert command. Used for inserting nodes
    //! and edges, inserting values to eisting nodes
    //! and edges (elements) as well as updating the
    //! existing values (overwriting).
    friend auto insert() noexcept -> Insert;

private:
    template<typename DataT>
    explicit Query(DataT data) noexcept :
        mData{std::move(data)}
    {
    }

    QueryData mData;
    std::vector<Placeholder> mPlaceholders;
};

class Query::Insert
{
public:
    [[nodiscard]] auto node() -> Query
    {
        return Query{InsertNodesCount{1}};
    }

    [[nodiscard]] auto node(std::vector<adb::KeyValue> values) -> Query
    {
        return Query{InsertNodeValues{std::move(values)}};
    }

    [[nodiscard]] auto node(Query query) -> Query
    {
        return Query{InsertNodeQuery{std::make_unique<Query>(std::move(query))}};
    }

    [[nodiscard]] auto node(const char *placeholderName) -> Query
    {
        Query query{InsertNodeValues{}};
        const auto handler = [](PlaceholderValue &&value, QueryData *data) {
            std::get<InsertNodeValues>(*data).values = std::move(std::get<std::vector<adb::KeyValue>>(value));
        };
        query.mPlaceholders.emplace_back(Placeholder{placeholderName, handler});
        return query;
    }

    [[nodiscard]] auto nodes(acore::size_type count) -> Query
    {
        return Query{InsertNodesCount{count}};
    }

    [[nodiscard]] auto nodes(std::vector<std::vector<adb::KeyValue>> values) -> Query
    {
        return Query{InsertNodesValues{std::move(values)}};
    }

    [[nodiscard]] auto nodes(Query query) -> Query
    {
        return Query{InsertNodesQuery{std::make_unique<Query>(std::move(query))}};
    }
};

[[nodiscard]] inline auto insert() noexcept -> Query::Insert
{
    return Query::Insert{};
}
}

#endif
