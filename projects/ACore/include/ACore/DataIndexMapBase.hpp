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

#include "ACoreModule.hpp"
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
//! To manipulate the data use insert(), remove() and value()
//! methods.
//!
//! It is also possible to get all the values associated
//! with a given \c element using the values() method or
//! only keys() with the namesake method.
//!
//! The \c Data template argument must provide the
//! following types and functionality:
//!
//! Methods
//! - <tt> auto %clear() -> void </tt>
//! - <tt> auto %count() const -> acore::size_type </tt>
//! - <tt> auto %count(acore::size_type element) const -> acore::size_type </tt>
//! - <tt> auto %insert(acore::size_type element, acore::size_type key, acore::size_type value) -> void </tt>
//! - <tt> auto %remove(acore::size_type element) -> void </tt>
//! - <tt> auto %remove(acore::size_type element, acore::size_type key) -> void </tt>
//! - <tt> auto %size() const -> acore::size_type </tt>
//! - <tt> auto shrink_to_fit() -> void </tt>
//! - <tt> auto %value(acore::size_type element, acore::size_type key) const -> acore::size_type </tt>
//! - <tt> auto %values(acore::size_type element) const -> acore::std::vector<#acore::DataIndexMapElement> </tt>
template<typename Data>
class DataIndexMapBase
{
public:
    //! Clears all the data from this object.
    constexpr auto clear() -> void
    {
        mData.clear();
    }

    //! Returns the \c number of the \c element-key-value
    //! data stored in the object.
    [[nodiscard]] constexpr auto count() const noexcept -> size_type
    {
        return mData.count();
    }

    //! Returns the \c number of the \c key-value
    //! pairs associated with the \a element.
    [[nodiscard]] constexpr auto count(size_type element) const -> size_type
    {
        validateElement(element);

        if (isInBounds(element))
        {
            return mData.count(element);
        }

        return 0;
    }

    //! Inserts or updates the \c element-key-value
    //! data. If there already is a value associated
    //! with the \c element-key pair it is updated.
    constexpr auto insert(size_type element, size_type key, size_type value) -> void
    {
        validateElement(element);
        mData.insert(element, key, value);
    }

    //! Returns \c true if there is no \c element-key-value
    //! data stored in the map.
    [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool
    {
        return count() == 0;
    }

    //! Returns \c list of keys associated with the
    //! \a element or empty list if the \a element
    //! has no data stored in the map. The \a element
    //! must be 0 or positive value.
    [[nodiscard]] auto keys(size_type element) const -> std::vector<size_type>
    {
        validateElement(element);

        if (isInBounds(element))
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

        return {};
    }

    //! Removes all data associated  with the \a element.
    constexpr auto remove(size_type element) -> void
    {
        validateElement(element);

        if (isInBounds(element))
        {
            mData.remove(element);
        }
    }

    //! Removes the single value associated with
    //! the \c element-key pair from the map.
    constexpr auto remove(size_type element, size_type key) -> void
    {
        validateElement(element);

        if (isInBounds(element))
        {
            mData.remove(element, key);
        }
    }

    //! Releases all elements that no longer
    //! have nay associated values. Call this
    //! function to free up resources after
    //! removing the elements at end of the
    //! used range.
    auto shrink_to_fit() -> void
    {
        mData.shrink_to_fit();
    }

    //! Returns the number of elements in the map.
    //! Useful for iteration as it serves as the upper
    //! bound to the elements' values.
    [[nodiscard]] constexpr auto size() const noexcept -> size_type
    {
        return mData.size();
    }

    //! Returns the mutable pointer to the instance
    //! of the \c Data template argument used as
    //! the internal storage.
    [[nodiscard]] constexpr auto storage() noexcept -> Data *
    {
        return &mData;
    }

    //! Returns the immutable pointer to the instance
    //! of the \c Data template argument used as
    //! the internal storage.
    [[nodiscard]] constexpr auto storage() const noexcept -> const Data *
    {
        return &mData;
    }

    //! Returns the \c value associated with the
    //! \c element-key pair or throws an #Exception
    //! if there is no value associated with the
    //! \a key - \a value pair.
    [[nodiscard]] constexpr auto value(size_type element, size_type key) const -> size_type
    {
        validateElementBounds(element);
        return mData.value(element, key);
    }

    //! Returns the list of the DataIndexMapElement
    //! associated with the \a element. The \a element
    //! must be 0 or positive value.
    [[nodiscard]] auto values(size_type element) const -> std::vector<DataIndexMapElement>
    {
        validateElement(element);

        if (isInBounds(element))
        {
            return mData.values(element);
        }

        return {};
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

    auto validateElementBounds(size_type element) const -> void
    {
        validateElement(element);

        if (!isInBounds(element))
        {
            throw Exception{} << "The 'element' must be in bounds (0 - " << size() << ')';
        }
    }

    [[nodiscard]] constexpr auto isInBounds(size_type element) const noexcept -> bool
    {
        return element < size();
    }

    Data mData;
};
}

#endif
