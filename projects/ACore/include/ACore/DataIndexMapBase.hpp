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

#ifndef ACORE_ACOREDATAINDEXMAPBASE_HPP
#define ACORE_ACOREDATAINDEXMAPBASE_HPP

#include "DataIndexMapElement.hpp"
#include "Exception.hpp"

#include <vector>

namespace acore
{
//! The DataIndexMapBase is the template class that
//! provides \c element-key-value associative
//! container of the acore::size_type values.
//!
//! This special purpose container is primarily intended
//! to associate values stored in the Dictionary class
//! with the elements of various data structures such
//! as graphs. All of the elements, keys and values
//! are identified using the acore::size_type values.
//!
//! Besides the convenience methods to manipulate
//! the data with insert(), remove() and value()
//! it is also possible to iterate over all the stored
//! data.
//!
//! It is also possible to get all the values associated
//! with the given \c element using the operator[]
//! or values() methods.
//!
//! The \c Data template argument must provide the
//! following types and functionality:
//!
//! Types
//! - <tt>Data::const_iteartor </tt>
//!
//! Methods
//! - <tt> Data::const_iterator %begin() const </tt>
//! - <tt> void %clear() </tt>
//! - <tt> acore::size_type %count() const </tt>
//! - <tt> acore::size_type %count(acore::size_type element) const </tt>
//! - <tt> Data::const_iterator %end() const </tt>
//! - <tt> Data::const_iterator %find(size_type element, size_type key) const </tt>
//! - <tt> void %insert(acore::size_type element, acore::size_type key, acore::size_type value) </tt>
//! - <tt> void %remove(acore::size_type element) </tt>
//! - <tt> void %remove(acore::size_type element, acore::size_type key) </tt>
//! - <tt> void %setValue(acore::size_type element, acore::size_type data, acore::size_type value) </tt>
//! - <tt> acore::size_type %value(acore::size_type element, acore::size_type key) const </tt>
//! - <tt> acore::std::vector<#acore::DataIndexMapElement> %values(acore::size_type element) const </tt>
//!
//! See DataIndexMapData class for example implementation.
template<typename Data>
class DataIndexMapBase
{
public:
    //! Clears all the data from this object.
    constexpr void clear()
    {
        mData.clear();
    }

    //! Returns the \c number of the \c element-key-value
    //! data stored in the object.
    [[nodiscard]] constexpr size_type count() const noexcept
    {
        return mData.count();
    }

    //! Returns the \c number of the \c key-value
    //! pairs associated with the \a element.
    [[nodiscard]] constexpr size_type count(size_type element) const
    {
        return mData.count(element);
    }

    //! Inserts or updates the \c element-key-value
    //! data. If there already is a value associated
    //! with the \c element-key pair it is updated.
    constexpr void insert(size_type element, size_type key, size_type value)
    {
        mData.insert(element, key, value);
    }

    //! Returns \c true if there is no \c element-key-value
    //! data stored in the map.
    [[nodiscard]] constexpr bool isEmpty() const noexcept
    {
        return count() == 0;
    }

    //! Returns \c list of keys associated with the
    //! \a element or empty list if the \a element
    //! has not data stored in the map.
    [[nodiscard]] std::vector<size_type> keys(size_type element) const
    {
        const std::vector<DataIndexMapElement> data = values(element);
        std::vector<size_type> keys;
        keys.reserve(data.size());

        for (const DataIndexMapElement &e : data)
        {
            keys.push_back(e.key);
        }

        return keys;
    }

    //! Removes all data associated  with the \a element.
    constexpr void remove(size_type element)
    {
        mData.remove(element);
    }

    //! Removes the single value associated with
    //! the \c element-key pair from the map.
    constexpr void remove(size_type element, size_type key)
    {
        mData.remove(element, key);
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type
    {
        return mData.size();
    }

    //! Returns the mutable pointer to the instance
    //! of the \c Data template argument used as
    //! the internal storage.
    [[nodiscard]] constexpr Data *storage() noexcept
    {
        return &mData;
    }

    //! Returns the immutable pointer to the instance
    //! of the \c Data template argument used as
    //! the internal storage.
    [[nodiscard]] constexpr const Data *storage() const noexcept
    {
        return &mData;
    }

    //! Returns the \c value associated with the
    //! \c element-key pair or the acore::INVALID_INDEX
    //! if the \c element-key pair does not exist.
    [[nodiscard]] constexpr size_type value(size_type element, size_type key) const
    {
        return mData.value(element, key);
    }

    //! Returns the list of the DataIndexMapElement
    //! associated with the \a element.
    [[nodiscard]] std::vector<DataIndexMapElement> values(size_type element) const
    {
        return mData.values(element);
    }

protected:
    //! Constructs the map and passes the \a args
    //! to the constructor of the internal \c Data.
    template<typename... Args>
    explicit constexpr DataIndexMapBase(Args... args) :
        mData(args...)
    {
    }

private:
    static auto validateElement(size_type element) -> void
    {
        if (element < 0)
        {
            throw Exception{} << "The 'element' must not be negative ('" << element << "' given)";
        }
    }

    Data mData;
};
}

#endif
