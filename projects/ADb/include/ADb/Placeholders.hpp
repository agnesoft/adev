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

#ifndef ADB_PLACEHOLDERS_HPP
#define ADB_PLACEHOLDERS_HPP

#include "QueryData.hpp"

namespace adb
{
//! \relates adb::Query
//! Types of values that can be bound to a query
//! data.
using PlaceholderValue = std::variant<acore::size_type,
                                      std::vector<acore::size_type>,
                                      std::vector<adb::KeyValue>,
                                      std::vector<std::vector<adb::KeyValue>>>;

//! \relates adb::Query
//! Wrapper around the name of placeholder.
struct Placeholder
{
    struct Count;
    struct Id;
    struct Ids;
    struct Values;
    struct MultiValues;

    //! Constructs the placeholder with name \a n.
    explicit Placeholder(std::string n) :
        name(std::move(n))
    {
    }

    //! Placeholder's name.
    std::string name;
};

//! \relates adb::Query
//! Used when the placeholder represents count.
struct Placeholder::Count : Placeholder
{
    using Placeholder::Placeholder;
};

//! \relates adb::Query
//! Used when the placeholder represents an element
//! id.
struct Placeholder::Id : Placeholder
{
    using Placeholder::Placeholder;
};

//! \relates adb::Query
//! Used when the placeholder represents an elements'
//! ids.
struct Placeholder::Ids : Placeholder
{
    using Placeholder::Placeholder;
};

//! \relates adb::Query
//! Used when the placeholder represents single
//! element's values.
struct Placeholder::Values : Placeholder
{
    using Placeholder::Placeholder;
};

//! \relates adb::Query
//! Used when the placeholder represents multiple
//! elements' values.
struct Placeholder::MultiValues : Placeholder
{
    using Placeholder::Placeholder;
};

//! \cond IMPLEMENTAION_DETAIL
using BindPlaceholderFunction = auto (*)(PlaceholderValue &&value, QueryData *data) -> void;

inline auto bindInsertNodeValues(PlaceholderValue &&value, QueryData *data)
{
    std::get<InsertNodeData>(*data).values = std::vector<std::vector<adb::KeyValue>>{std::move(std::get<std::vector<adb::KeyValue>>(value))};
}

inline auto bindInsertNodesValues(PlaceholderValue &&value, QueryData *data) -> void
{
    std::get<InsertNodeData>(*data).count = static_cast<acore::size_type>(std::get<std::vector<std::vector<adb::KeyValue>>>(value).size());
    std::get<InsertNodeData>(*data).values = std::move(std::get<std::vector<std::vector<adb::KeyValue>>>(value));
}

inline auto bindInsertNodesCount(PlaceholderValue &&value, QueryData *data) -> void
{
    std::get<InsertNodeData>(*data).count = std::get<acore::size_type>(value);
}

inline auto bindInsertEdgeFrom(PlaceholderValue &&value, QueryData *data) -> void
{
    std::get<InsertEdgeData>(*data).from = std::vector<acore::size_type>{std::get<acore::size_type>(value)};
}

inline auto bindInsertEdgesFrom(PlaceholderValue &&value, QueryData *data) -> void
{
    std::get<InsertEdgeData>(*data).from = std::move(std::get<std::vector<acore::size_type>>(value));
}

inline auto bindInsertEdgeTo(PlaceholderValue &&value, QueryData *data) -> void
{
    std::get<InsertEdgeData>(*data).to = std::vector<acore::size_type>{std::get<acore::size_type>(value)};
}

inline auto bindInsertEdgesTo(PlaceholderValue &&value, QueryData *data) -> void
{
    std::get<InsertEdgeData>(*data).to = std::move(std::get<std::vector<acore::size_type>>(value));
}

inline auto bindInsertEdgeValues(PlaceholderValue &&value, QueryData *data) -> void
{
    std::get<InsertEdgeData>(*data).values = std::vector<std::vector<adb::KeyValue>>{std::move(std::get<std::vector<adb::KeyValue>>(value))};
}

inline auto bindInsertEdgesValues(PlaceholderValue &&value, QueryData *data) -> void
{
    std::get<InsertEdgeData>(*data).values = std::move(std::get<std::vector<std::vector<adb::KeyValue>>>(value));
}

inline auto bindInsertEdgesCount(PlaceholderValue &&value, QueryData *data) -> void
{
    std::get<InsertEdgeData>(*data).count = std::get<acore::size_type>(value);
}

struct PlaceholderData
{
    std::string name;
    BindPlaceholderFunction bind;
};
//! \endcond
}

#endif
