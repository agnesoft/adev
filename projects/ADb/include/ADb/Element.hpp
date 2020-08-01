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

#ifndef ADB_ELEMENT_HPP
#define ADB_ELEMENT_HPP

#include "ADbModule.hpp"
#include "KeyValue.hpp"

#include <vector>

namespace adb
{
//! The Element provides the access to index and
//! data of the Database's nodes and edges.
//!
//! The element's index represents the node's or edge's
//! index in the database. The data are the key-value
//! pairs associated with it in the Database.
class Element
{
public:
    //! Default constructor.
    Element() = default;

    //! Constructs the Element with \a index and
    //! no data.
    explicit Element(acore::size_type index) :
        mIndex{index}
    {
    }

    //! Constructs the Element with \a index and
    //! \a data.
    Element(acore::size_type index, std::vector<KeyValue> data) :
        mIndex{index},
        mData{std::move(data)}
    {
    }

    //! Returns the element's data.
    [[nodiscard]] constexpr auto data() const noexcept -> const std::vector<KeyValue> &
    {
        return mData;
    }

    //! Returns the index of the database element
    //! this object represents. If the value is
    //! invalid or was default constructed acore::INVALID_INDEX
    //! is returned.
    [[nodiscard]] constexpr auto index() const noexcept -> acore::size_type
    {
        return mIndex;
    }

private:
    acore::size_type mIndex = acore::INVALID_INDEX;
    std::vector<KeyValue> mData;
};

//! \relates adb::Element
//! Returns \c true if the \a left represents the
//! same element as \a right with the same index
//! and data.
[[nodiscard]] constexpr auto operator==(const Element &left, const Element &right) -> bool
{
    return left.index() == right.index() && left.data() == right.data();
}

//! \relates adb::Element
//! Returns \c true if the \a left does not represent
//! the same element as \a right (either the index
//! or data differ).
[[nodiscard]] constexpr auto operator!=(const Element &left, const Element &right) -> bool
{
    return !(left == right);
}
}

#endif
