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

#ifndef ADB_VALUE_HPP
#define ADB_VALUE_HPP

#include "ADbModule.hpp"

#include <array>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace adb
{
//! The Value represents a value in the database.
//! It can be constructed with any value type as
//! long as either:
//!
//! - <tt> std::is_trivially_copyable_v<T> == true </tt>
//! - <tt> (sizeof(T) <= sizeof(acore::size_type) == true </tt>
//!
//! or the following operators are provided:
//!
//! - <tt> auto operator<<(acore::DataStream &stream, const T &value) </tt>
//! - <tt> auto operator>>(acore::DataStream &stream, T &value) </tt>
//!
//! In the former case the value is simply copied
//! to the internal buffer and copied back when
//! requested. This also applies to the string types
//! (std::string, std::string_view and const char *)
//! provided their size is less than \c sizeof(acore::size_type).
//!
//! In the latter case the value is serialized and
//! deserialized as requested. This way any data
//! can be stored into and retrieved from the
//! database.
//!
//! The Value class performs static (compile time)
//! sanity checking when the values are requested.
//! It will throw when a value is requested from
//! a default constructred object, a trivial value
//! is requested from an object storing a complex
//! value and vice versa. No runtime type safety
//! checks are performed though apart from buffer
//! overrun during the (de)serialization.
class Value
{
    using TrivialValue = std::array<char, sizeof(acore::size_type)>;

public:
    //! Default constructs the Value. Such an object
    //! will throw when you attemp to call get().
    Value() = default;

    //! Constructs the Value with \a value. For
    //! trivial types it will simply copy them into
    //! the internal buffer. For complex types it
    //! will serialize them.
    template<typename T>
    explicit constexpr Value(const T &value)
    {
        if constexpr (std::is_trivially_copyable_v<T> && sizeof(T) <= sizeof(acore::size_type))
        {
            mData = save(&value, sizeof(value));
        }
        else
        {
            mData = acore::Variant{value};
        }
    }

    //! Constructs the Value with \a value. If the
    //! size of the string is less than or equal to
    //! the internal buffer <tt> sizeof(acore::size_type) </tt>
    //! the string will be copied into it. Otherwise
    //! it will be serialized.
    explicit Value(std::string_view value)
    {
        if (value.size() <= sizeof(acore::size_type))
        {
            mData = save(value.data(), value.size());
        }
        else
        {
            mData = acore::Variant{value};
        }
    }

    //! Same as Value(std::string_view).
    explicit Value(const char *value) :
        Value(std::string_view{value})
    {
    }

    //! Same as Value(std::string_view).
    explicit Value(const std::string &value) :
        Value(std::string_view{value})
    {
    }

    //! Retrieves the object of type T from the Value.
    //!
    //! If the Value holds a trivial data simple
    //! std::memcpy to T will be performed.
    //!
    //! If the Value holds a complex data the T
    //! is deserialized.
    //!
    //! If the Value was default consttructed or
    //! the type mismatch occurs this function will
    //! throw.
    template<typename T>
    [[nodiscard]] auto get() const -> T
    {
        if constexpr (std::is_trivially_copyable_v<T> && sizeof(T) <= sizeof(acore::size_type))
        {
            return std::visit([](auto &&value) -> T {
                using DataT = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<DataT, std::monostate>)
                {
                    throw acore::Exception{} << "The adb::Value has no value.";
                }
                else if constexpr (std::is_same_v<DataT, TrivialValue>)
                {
                    T val;
                    std::memcpy(&val, value.data(), sizeof(T));
                    return val;
                }
                else
                {
                    throw acore::Exception{} << "Data stroed as 'acore::Variant' but trivial '" << typeid(T).name() << "' requested";
                }
            },
                              mData);
        }
        else
        {
            return std::visit([](auto &&value) -> T {
                using DataT = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<DataT, std::monostate>)
                {
                    throw acore::Exception{} << "The adb::Value has no value.";
                }
                else if constexpr (std::is_same_v<DataT, TrivialValue>)
                {
                    throw acore::Exception{} << "Data stored as trivial value but non-trivial '" << typeid(T).name() << "' requested";
                }
                else
                {
                    return value.template value<T>();
                }
            },
                              mData);
        }
    }

private:
    template<typename T>
    [[nodiscard]] auto save(const T *value, size_t size) -> TrivialValue
    {
        TrivialValue val{};
        std::memcpy(val.data(), value, size);
        return val;
    }

    [[nodiscard]] friend auto operator==(const Value &left, const Value &right) noexcept -> bool
    {
        return left.mData == right.mData;
    }

    [[nodiscard]] friend auto operator!=(const Value &left, const Value &right) noexcept -> bool
    {
        return !(left == right);
    }

    std::variant<std::monostate, TrivialValue, acore::Variant> mData;
};

//! Retrieves the string from the internal data.
//!
//! If the value is storing trivial data the
//! returned std::string_view will be a view
//! into this data buffer possibly shortened to
//! the last null '\0' character found (if any).
//!
//! If the value is storing a complex data the
//! returned std::string_view will be a view
//! into the internal serialization buffer (see
//! acore::Variant for further details).
//!
//! If the value was default constructred this
//! function will throw.
template<>
[[nodiscard]] auto Value::get() const -> std::string_view
{
    return std::visit([](auto &&value) -> std::string_view {
        using DataT = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<DataT, std::monostate>)
        {
            throw acore::Exception{} << "The adb::Value has no value.";
        }
        else if constexpr (std::is_same_v<DataT, TrivialValue>)
        {
            const auto valueSize = [](const TrivialValue &value) -> size_t {
                for (size_t i = value.size() - 1; i < value.size(); --i)
                {
                    if (value[i] != '\0')
                    {
                        return i + 1;
                    }
                }

                return 0;
            };

            return std::string_view{value.data(), valueSize(value)};
        }
        else
        {
            return value.template value<std::string_view>();
        }
    },
                        mData);
}

//! Same as get<std::string_view>() but returns
//! a copy of the string.
template<>
[[nodiscard]] auto Value::get() const -> std::string
{
    return std::string{get<std::string_view>()};
}
}

#endif
