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

#include "Placeholders.hpp"
#include "QueryData.hpp"

namespace adb
{
class Query;

//! Function to use to bind the sub-query's result
//! to the query's data.
using BindResultFunction = BindPlaceholderFunction;

//! The SubQuery struct represents a sub-query of
//! an adb::Query. It holds the sub-query itself
//! and the function used to bind the sub-query's
//! result to the parent query's data.
struct SubQuery
{
    //! Constructs the SubQuery with \a subQuery
    //! and \a bindFunction.
    SubQuery(Query &&subQuery, BindResultFunction bindFunction);

    //! Copies this sub-query. Provided so that the
    //! adb::Query class is copyable.
    //!
    //! \note This constructor performs deep
    //! recursive copy of the internal query.
    SubQuery(const SubQuery &other);

    //! Move constructor.
    SubQuery(SubQuery &&other) noexcept = default;

    //! Destructor.
    ~SubQuery();

    //! Copy assignment operator. Provided so that
    //! the adb::Query class is copy assignable.
    //!
    //! \note This operator performs deep recursive
    //! copy of the internal query.
    auto operator=(const SubQuery &other) -> SubQuery &;

    //! Move assignment operator.
    auto operator=(SubQuery &&other) noexcept -> SubQuery & = default;

    [[nodiscard]] auto bind() const noexcept -> BindResultFunction;

    [[nodiscard]] auto query() const noexcept -> Query *;

private:
    //! Pointer to the query.
    std::unique_ptr<Query> mQuery;

    //! Function to bind the query's result to the
    //! parent adb::Query.
    BindResultFunction mBind = nullptr;
};
}
#endif
