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
#include "Value.hpp"

#include <utility>
#include <variant>
#include <vector>

namespace adb
{
//! The Query class represents a query to the database.
//! Queries are constructed using fluent interface
//! in order to provide semantic correctness (not
//! necessarily data correctness) and readability
//! to writing queries.
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
class Query
{
public:
    class Insert;

    //! Data struct representing data of the insert
    //! nodes query.
    struct InsertNodes
    {
        //! Node values to be inserted into the database.
        std::vector<std::vector<adb::KeyValue>> values;
    };

    //! Alias for all the possible query data types.
    using Data = std::variant<std::monostate,
                              InsertNodes>;

    //! Returns the data of the query. Used by the
    //! database to access the query's data to execute.
    [[nodiscard]] auto data() const noexcept -> const Data &
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

    Data mData;
};

//! The Query::Insert class servers as the entry
//! point for building the insert queries.
class Query::Insert
{
public:
    //! Creates a query that inserts a single node
    //! with no data.
    [[nodiscard]] auto node() -> Query
    {
        return Query{InsertNodes{std::vector<std::vector<adb::KeyValue>>(1)}};
    }

    //! Creates a query that inserts a single node
    //! with \a values associated with it.
    [[nodiscard]] auto node(std::vector<adb::KeyValue> values) -> Query
    {
        return Query{InsertNodes{{std::move(values)}}};
    }

    //! Creates a query that inserts \a count number
    //! of nodes with no data.
    [[nodiscard]] auto nodes(acore::size_type count) -> Query
    {
        return Query{InsertNodes{std::vector<std::vector<adb::KeyValue>>(count)}};
    }

    //! Creates a query that inserts \a values.size()
    //! nodes with \a values. Each entry in \a values
    //! represents values of each inserted node.
    [[nodiscard]] auto nodes(std::vector<std::vector<adb::KeyValue>> values) -> Query
    {
        return Query{InsertNodes{std::move(values)}};
    }
};

[[nodiscard]] inline auto insert() noexcept -> Query::Insert
{
    return Query::Insert{};
}
}

#endif
