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
#include "InsertQuery.hpp"
#include "KeyValue.hpp"
#include "Placeholders.hpp"
#include "QueryData.hpp"
#include "SelectQuery.hpp"
#include "SubQuery.hpp"
#include "Value.hpp"

#include <memory>
#include <string>
#include <string_view>
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
    class Select;

public:
    //! Binds a value of a placehodler associated
    //! with \a name to \a value. Note that if the
    //! \a value 's type does not match the type
    //! expected by the placeholding value an exception
    //! is thrown.
    auto bind(std::string_view name, PlaceholderValue value) -> void
    {
        const auto it = std::find_if(mPlaceholders.begin(), mPlaceholders.end(), [&](const auto &placeholder) {
            return placeholder.name == name;
        });

        if (it == mPlaceholders.end())
        {
            throw acore::Exception{} << "Placeholder '" << name << "' not found.";
        }

        it->bind(std::move(value), &mData);
    }

    //! Convenience overload for binding a list of
    //! values to a single element.
    auto bind(std::string_view name, std::vector<adb::KeyValue> values) -> void
    {
        bind(name, PlaceholderValue{std::move(values)});
    }

    //! Convenience overload for binding a list of
    //! values to multiple elements.
    auto bind(std::string_view name, std::vector<std::vector<adb::KeyValue>> values) -> void
    {
        bind(name, PlaceholderValue{std::move(values)});
    }

    //! Returns the data of the query. Used by the
    //! database to access the query's data to execute.
    [[nodiscard]] auto data() const noexcept -> const QueryData &
    {
        return mData;
    }

    //! Returns the list of sub queries of this query.
    [[nodiscard]] auto subQueries() const noexcept -> const std::vector<SubQuery> &
    {
        return mSubQueries;
    }

private:
    friend class InsertQuery;
    friend class InsertEdgeQuery;
    friend class InsertEdgeFromQuery;
    friend class SelectQuery;

    template<typename DataT>
    explicit Query(DataT data) noexcept :
        mData{std::move(data)}
    {
    }

    auto addPlaceholder(std::string placeholder, BindPlaceholderFunction bindFunction) -> void
    {
        const auto it = std::find_if(mPlaceholders.begin(), mPlaceholders.end(), [&](const auto &p) {
            return p.name == placeholder;
        });

        if (it != mPlaceholders.end())
        {
            throw acore::Exception{} << "Placeholder '" << placeholder << "' already exists.";
        }

        mPlaceholders.push_back({std::move(placeholder), bindFunction});
    }

    auto addSubQuery(Query query, BindResultFunction bindFunction) -> void
    {
        mSubQueries.push_back({std::move(query), bindFunction});
    }

    QueryData mData;
    std::vector<Placeholder> mPlaceholders;
    std::vector<SubQuery> mSubQueries;
};

//! Base class of specialized query wrappers that
//! indicate the result type.
class BaseQuery : public Query
{
public:
    //! Constructs the query by moving from \a query.
    BaseQuery(Query &&query) :
        Query{std::move(query)}
    {
    }
};

//! Wrapper around adb::Query indicating that the
//! result of the query will be a \c count value.
//! Typically used as a sub query where \c count is
//! the expected argument.
class CountQuery : public BaseQuery
{
public:
    using BaseQuery::BaseQuery;
};

//! Wrapper around adb::Query indicating that the
//! result of the query will be a single \c id.
//! Typically used as a sub query where \c id is
//! the expected argument.
class IdQuery : public BaseQuery
{
public:
    using BaseQuery::BaseQuery;
};

//! Wrapper around adb::Query indicating that the
//! result of the query will be a list of \c ids.
//! Typically used as a sub query where the
//! \c ids are the expected argument.
class IdsQuery : public BaseQuery
{
public:
    using BaseQuery::BaseQuery;
};

//! Wrapper around adb::Query indicating that the
//! result of the query will be a \c values of
//! multiple elements. Typically used as a sub query
//! where \c values are the expected argument.
class MultiValuesQuery : public BaseQuery
{
public:
    using BaseQuery::BaseQuery;
};

//! Wrapper around adb::Query indicating that the
//! result of the query will be a \c values.
//! Typically used as a sub query where \c values
//! are the expected argument.
class ValuesQuery : public BaseQuery
{
public:
    using BaseQuery::BaseQuery;
};

//! \relates adb::Query
//! Stand alone function that begins composing
//! the insert query. Used for inserting nodes
//! and edges, inserting values to eisting nodes
//! and edges (elements) as well as updating the
//! existing values (overwriting).
[[nodiscard]] inline auto insert() noexcept -> InsertQuery
{
    return InsertQuery{};
}

//! \relates adb::Query
//! Stand alone function that begins composing
//! the select query. Used for selecting data,
//! meta data, aggregate data etc. from the
//! database.
[[nodiscard]] inline auto select() noexcept -> SelectQuery
{
    return SelectQuery{};
}
}

#endif
