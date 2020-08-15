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

#ifndef ADB_QUERYDATA_HPP
#define ADB_QUERYDATA_HPP

#include "ADbModule.hpp"
#include "KeyValue.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace adb
{
//! \cond IMPLEMENTAION_DETAIL
struct InsertEdgeData
{
    enum class Type : char
    {
        Assymmetric,
        Symmetric
    };

    acore::size_type count = 0;
    std::vector<acore::size_type> from;
    std::vector<acore::size_type> to;
    std::vector<std::vector<adb::KeyValue>> values;
    Type type = Type::Symmetric;
};

struct InsertNodeData
{
    acore::size_type count = 0;
    std::vector<std::vector<adb::KeyValue>> values;
};

struct SelectData
{
};
//! \endcond

//! \relates adb::Query
//! Represents the possible internal data types
//! of the database query.
using QueryData = std::variant<InsertEdgeData,
                               InsertNodeData,
                               SelectData>;
}

#endif
