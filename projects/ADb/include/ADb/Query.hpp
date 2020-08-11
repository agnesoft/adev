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

#include <memory>
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
    //! \cond IMPLEMENTAION_DETAIL
    class Insert;

    struct SubQuery
    {
        SubQuery() = default;

        SubQuery(std::unique_ptr<Query> subQuery) :
            query{std::move(subQuery)}
        {
        }

        SubQuery(const SubQuery &other)
        {
            *this = other;
        }

        SubQuery(SubQuery &&other) noexcept = default;
        ~SubQuery() = default;

        auto operator=(const SubQuery &other) -> SubQuery &
        {
            query = std::make_unique<Query>(*other.query);
            return *this;
        }

        auto operator=(SubQuery &&other) noexcept -> SubQuery & = default;

        std::unique_ptr<Query> query;
    };

    struct InsertNodeQuery : SubQuery
    {
    };

    struct InsertNodeValues
    {
        std::vector<adb::KeyValue> values;
    };

    struct InsertNodesCount
    {
        acore::size_type count = 0;
    };

    struct InsertNodesQuery : SubQuery
    {
    };

    struct InsertNodesValues
    {
        std::vector<std::vector<adb::KeyValue>> values;
    };
    //! \endcond

    //! Alias for all the possible query data types.
    using Data = std::variant<std::monostate,
                              InsertNodeQuery,
                              InsertNodeValues,
                              InsertNodesCount,
                              InsertNodesQuery,
                              InsertNodesValues>;

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

//! \cond IMPLEMENTAION_DETAIL
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

    [[nodiscard]] auto
        nodes(acore::size_type count) -> Query
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
//! \endcond
}

#endif
