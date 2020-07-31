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
//! The KeyValue represents a single piece of data
//! (a key-value pair) associated with the database's
//! elements (nodes and edges). It provides convenience
//! constructor in order to work with the Value objects
//! easier when used within the Database context.
class KeyValue
{
public:
    //! Default constructor.
    KeyValue() = default;

    //! Constructs the KeyValue with \a key and
    //! \a value. Provided to avoid too verbose
    //! syntax:
    //!
    //! \snippet KeyValueTest.cpp [Verbose]
    //!
    //! and to allow:
    //!
    //! \snippet KeyValueTest.cpp [Brief]
    //!
    //! and constructing a vector:
    //!
    //! \snippet KeyValueTest.cpp [Vector]
    template<typename KeyT, typename ValueT>
    KeyValue(const KeyT &key, const ValueT &value) :
        mKey{key},
        mValue{value}
    {
    }

    //! Returns the key.
    [[nodiscard]] auto key() const noexcept -> const Value &
    {
        return mKey;
    }

    //! Returns the value.
    [[nodiscard]] auto value() const noexcept -> const Value &
    {
        return mValue;
    }

private:
    Value mKey;
    Value mValue;
};

//! \relates adb::KeyValue
//! Returns \c true if both \a left and \a right
//! represents the same key-value pair or \c false
//! otherwise.
[[nodiscard]] inline bool operator==(const KeyValue &left, const KeyValue &right)
{
    return left.key() == right.key() && left.value() == right.value();
}

//! \relates adb::KeyValue
//! Returns \c true if \a left and \a right do not
//! represent the same key-value pair or \c false
//! otherwise.
[[nodiscard]] inline bool operator!=(const KeyValue &left, const KeyValue &right)
{
    return !(left == right);
}
}

#endif
