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

#ifndef ACORE_ACOREDATAINDEXMAPELEMENT_HPP
#define ACORE_ACOREDATAINDEXMAPELEMENT_HPP

#include "ACoreModule.hpp"
#include "DataStreamBase.hpp"

namespace acore
{
//! The DataIndexMapElement provides the common
//! data structure for DataIndexMapBase class and
//! implementations of its \c Data template
//! argument.
struct DataIndexMapElement
{
    //! Represents the element \c key.
    size_type key = INVALID_INDEX;
    //! Represents the element \c value.
    size_type value = INVALID_INDEX;
};

//! \relates DataIndexMapElement
//! Returns \c true if the \a left's key is less than
//! \a right 's. If they are equal returns \c true
//! if \a left 's value is less than \a right 's.
[[nodiscard]] constexpr bool operator<(DataIndexMapElement left, DataIndexMapElement right) noexcept
{
    if (left.key == right.key)
    {
        return left.value < right.value;
    }

    return left.key < right.key;
}

//! \relates DataIndexMapElement
//! Returns \c true if the \a left 's key and value
//! are equal to the \a right 's.
[[nodiscard]] constexpr bool operator==(DataIndexMapElement left, DataIndexMapElement right) noexcept
{
    return left.key == right.key && left.value == right.value;
}

//! \relates DataIndexMapElement
//! Returns \c true if the \a left 's key or value
//! are different to the \a right 's.
[[nodiscard]] constexpr bool operator!=(DataIndexMapElement left, DataIndexMapElement right) noexcept
{
    return !(left == right);
}

//! \relates DataIndexMapElement
//! Serializes the \a element into the \a stream as
//! the \c key followed by \a value  and returns the
//! \c stream.
template<typename Buffer>
constexpr DataStreamBase<Buffer> &operator<<(DataStreamBase<Buffer> &stream, const DataIndexMapElement &element)
{
    return stream << element.key << element.value;
}

//! \relates DataIndexMapElement
//! Deserializes the \a element from the \a stream as
//! the \c key followed by \a value  and returns the
//! \c stream.
template<typename Buffer>
constexpr DataStreamBase<Buffer> &operator>>(DataStreamBase<Buffer> &stream, DataIndexMapElement &element)
{
    return stream >> element.key >> element.value;
}
}

#endif
