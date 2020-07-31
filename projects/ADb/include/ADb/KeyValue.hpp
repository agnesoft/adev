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

#ifndef ADB_KEYVALUE_HPP
#define ADB_KEYVALUE_HPP

#include "ADbModule.hpp"
#include "Value.hpp"

namespace adb
{
//! Struct representing single piece of data - a
//! key-value pair - associated with the database's
//! elements (nodes and edges).
struct KeyValue
{
    //! Key
    Value key;

    //! Value
    Value value;
};

//! Returns \c true if both \a left and \a right
//! represents the same key-value pair or \c false
//! otherwise.
[[nodiscard]] inline bool operator==(const KeyValue &left, const KeyValue &right)
{
    return left.key == right.key && left.value == right.value;
}

//! Returns \c true if \a left and \a right do not
//! represent the same key-value pair or \c false
//! otherwise.
[[nodiscard]] inline bool operator!=(const KeyValue &left, const KeyValue &right)
{
    return !(left == right);
}
}

#endif
