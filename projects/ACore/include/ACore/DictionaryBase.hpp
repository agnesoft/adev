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

#ifndef ACORE_DICTIONARYBASE_HPP
#define ACORE_DICTIONARYBASE_HPP

#include "ForwardIterator.hpp"
#include "Variant.hpp"

#include <functional>

namespace acore
{
//! Returns the hash of the value. This free function is
//! used in DictionaryBase and can be used in its \c Data
//! template argument implementations for consitency.
[[nodiscard]] static inline auto dictionaryValueHash(const Variant &value) -> size_type
{
    const std::vector<char> &val{value.value<const std::vector<char> &>()};
    return static_cast<size_type>(std::hash<std::string_view>{}(std::string_view(val.data(), static_cast<std::size_t>(val.size()))));
}

//! The DictionaryBase<Data> is a template class
//! that provides the reference counted size_type -
//! \c T dictionary.
//!
//! The DictionaryBase provides common interface and
//! algorithms for the reference counted size_type -
//! \c T dictionary. The dictionary provides
//! bi-directional lookup. The \c Data template
//! argument type  must implement the following
//! methods:
//!
//! - <tt> auto %capacity() const -> size_type</tt>
//! - <tt> auto %clear() -> void </tt>
//! - <tt> auto %count() const -> size_type </tt>
//! - <tt> auto %indexes(size_type hash) const -> std::vector<size_type> </tt>
//! - <tt> auto %insert(size_type hash, const Variant &value) -> size_type </tt>
//! - <tt> auto %remove(size_type index, size_type hash) -> void </tt>
//! - <tt> auto %value(size_type index) const -> Variant </tt>
//!
//! The size_type id representing the value reduces
//! the storage costs and allows fast lookup.
//!
//! The values to be insterted must either be trivial, i.e.:
//!
//! - <tt> Be <a href="https://en.cppreference.com/w/cpp/types/is_trivially_copyable">trivially-copyable</a> </tt>
//! - <tt> sizeof(T) <= sizeof(#size_type) </tt>
//!
//! In such case the value is not actually stored but
//! rather embedded into the returned index value.
//!
//! In all other cases the value is considered "complex"
//! and must:
//!
//! - Be convertible to and from the Variant
//!
//! When inserting each value is first looked up in
//! the dictionary. If it is found its index will
//! simply be returned so there cannot be any duplicates.
//! If the value is not found the new index is returned.
//!
//! However the dictionary also tracks references
//! made to every value. For example when the same
//! value is inserted twice its reference count
//! obtainable from count() would be \c 2. Conversely
//! when removing the value the reference count is
//! reduced every time and the value is only actually
//! removed when the reference counter reaches \c 0.
//!
//! The values can be retrieved using the templated
//! value() method or operator[] that returns the
//! value as Variant.
template<typename Data>
class DictionaryBase
{
public:
    //! The immutable iterator type.
    using const_iterator = ForwardIterator<Variant, Variant, const DictionaryBase>;

    //! Returns a #const_iterator pointing to the
    //! first value or end() if the dictionary is empty.
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator
    {
        return ++end();
    }

    //! Removes all the values from the dictionary.
    constexpr auto clear() -> void
    {
        mData.clear();
    }

    //! Returns \c true if the \a index points to a
    //! stored value in the dictionary.
    [[nodiscard]] constexpr auto contains(size_type index) const -> bool
    {
        return 0 <= index && index < mData.capacity() && mData.count(index) > 0;
    }

    //! Returns \c true if the \a value is stored in the
    //! dictionary.
    template<typename T>
    [[nodiscard]] constexpr auto containsValue(const T &value) const -> bool
    {
        return !std::is_trivially_copyable<T>::value && index(value) != INVALID_INDEX;
    }

    //! Returns \c number of values stored in the
    //! dictionary.
    [[nodiscard]] constexpr auto count() const noexcept -> size_type
    {
        return mData.count();
    }

    //! Returns \c reference \c count of a value associated
    //! with the \a index. If the \a index is not associated
    //! with a value \c 0 is returned.
    [[nodiscard]] constexpr auto count(size_type index) const -> size_type
    {
        if (contains(index))
        {
            return mData.count(index);
        }

        return 0;
    }

    //! Returns an invalid const_iterator.
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator
    {
        return const_iterator(INVALID_INDEX, this);
    }

    //! Returns \c index of the \a value if it is
    //! stored in the dictionary. If the \a value
    //! is not stored in the dictionary the
    //! acore::INVALID_INDEX is returned.
    template<typename T>
    [[nodiscard]] constexpr auto index(const T &value) const -> size_type
    {
        size_type idx = INVALID_INDEX;

        if constexpr (std::is_trivially_copyable<T>::value && sizeof(T) <= sizeof(size_type))
        {
            idx = 0;
            std::memcpy(&idx, &value, sizeof(T));
        }
        else
        {
            idx = findValue(value);
        }

        return idx;
    }

    //! Inserts the \a value into the dictionary and
    //! returns its \c index. If the \a value already
    //! exists in the dictionary its reference counter
    //! is incremented and its index is returned.
    //!
    //! If the \a value is <a href="https://en.cppreference.com/w/cpp/types/is_trivially_copyable">trivially-copyable</a>
    //! and sizeof(T) <= sizeof(#acore::size_type) the value
    //! is simply embedded into the returned id
    //! and not actally stored.
    template<typename T>
    constexpr auto insert(const T &value) -> size_type
    {
        size_type idx = 0;

        if constexpr (std::is_trivially_copyable<T>::value && sizeof(T) <= sizeof(size_type))
        {
            std::memcpy(&idx, &value, sizeof(T));
        }
        else
        {
            idx = insertValue(Variant{value});
        }

        return idx;
    }

    //! Returns \c true if the dictionary has no values.
    [[nodiscard]] constexpr auto isEmpty() const noexcept -> bool
    {
        return count() == 0;
    }

    //! Decrements the \c reference \c counter of
    //! the value associated with the \a index if
    //! it exists in the dictionary. If the counter
    //! reaches \c 0 the value is also removed from
    //! the dictioanry.
    constexpr auto remove(size_type index) -> void
    {
        if (contains(index))
        {
            if (mData.count(index) == 1)
            {
                mData.remove(index, dictionaryValueHash(mData.value(index)));
            }
            else
            {
                mData.remove(index);
            }
        }
    }

    //! Returns mutable \c Data *.
    [[nodiscard]] constexpr auto storage() noexcept -> Data *
    {
        return &mData;
    }

    //! Returns immutable \c Data *.
    [[nodiscard]] constexpr auto storage() const noexcept -> const Data *
    {
        return &mData;
    }

    //! Returns the \c value of type \c T stored at
    //! the \a index.
    //!
    //! If the \c T is the <a href="https://en.cppreference.com/w/cpp/types/is_trivially_copyable">trivially-copyable</a>
    //! type and sizeof(T) <= sizeof(#acore::size_type)
    //! it is simply reconstructed from the \a index.
    //!
    //! Otherwise is is retrieved as Variant and
    //! then converted to type \c T.
    //!
    //! If \a index does not point to a valid value
    //! an exception is thrown.
    template<typename T>
    [[nodiscard]] constexpr auto value(size_type index) const -> T
    {
        if constexpr (std::is_trivially_copyable<T>::value)
        {
            T val;
            std::memcpy(&val, &index, sizeof(T));
            return val;
        }
        else if (contains(index))
        {
            return mData.value(index).template value<T>();
        }
        else
        {
            throw Exception{} << "Index '" << index << "' does not exist in the dictionary.";
        }
    }

protected:
    //! Constructs the Dictionary passing \a args
    //! as unpacked arguments to \c Data.
    template<typename... T>
    explicit constexpr DictionaryBase(T... args) :
        mData(args...)
    {
    }

private:
    friend const_iterator;

    template<typename T>
    [[nodiscard]] auto findValue(const T &value) const -> size_type
    {
        const Variant val{value};
        return findValue(dictionaryValueHash(val), val);
    }

    [[nodiscard]] auto findValue(size_type hashValue, const Variant &value) const -> size_type
    {
        for (size_type i : mData.indexes(hashValue))
        {
            const Variant val{mData.value(i)};

            if (val.isValid() && val == value)
            {
                return i;
            }
        }

        return INVALID_INDEX;
    }

    [[nodiscard]] constexpr auto insertValue(const Variant &value) -> size_type
    {
        size_type hashValue = dictionaryValueHash(value);
        size_type idx = findValue(hashValue, value);

        if (idx == INVALID_INDEX)
        {
            idx = mData.insert(hashValue, value);
        }
        else
        {
            mData.insert(idx);
        }

        return idx;
    }

    [[nodiscard]] constexpr auto nextIndex(size_type index) const -> size_type
    {
        for (index++; index < mData.capacity(); index++)
        {
            if (contains(index))
            {
                return index;
            }
        }

        return INVALID_INDEX;
    }

    [[nodiscard]] auto referenceAt(size_type index) const -> Variant
    {
        return mData.value(index);
    }

    Data mData;
};
}

#endif
