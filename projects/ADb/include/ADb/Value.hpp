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

#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>

namespace adb
{
class Value
{
public:
    Value() = default;

    template<typename T>
    explicit constexpr Value(const T &value)
    {
        if constexpr (std::is_trivially_copyable_v<T> && sizeof(T) <= sizeof(mIndex))
        {
            memcpy(&mIndex, &value, sizeof(T));
        }
        else
        {
            mIndex = acore::INVALID_INDEX;
            mValue = acore::Variant{value};
        }
    }

    template<>
    explicit Value(const std::string_view &value)
    {
        if (value.size() <= sizeof(mIndex))
        {
            std::memcpy(&mIndex, &value[0], value.size());
        }
        else
        {
            mIndex = acore::INVALID_INDEX;
            mValue = acore::Variant{value};
        }
    }

    template<typename T>
    explicit constexpr operator T() const
    {
        if constexpr (std::is_trivially_copyable_v<T> && sizeof(T) <= sizeof(mIndex))
        {
            T val{};
            memcpy(&val, &mIndex, sizeof(T));
            return val;
        }
        else
        {
            return mValue.value<T>();
        }
    }

private:
    [[nodiscard]] auto isTrivial() const noexcept -> bool
    {
        return !mValue.isValid();
    }

    [[nodiscard]] friend auto operator==(const Value &left, const Value &right) noexcept -> bool
    {
        if (left.isTrivial())
        {
            if (right.isTrivial())
            {
                return left.mIndex == right.mIndex;
            }
        }
        else if (!right.isTrivial())
        {
            return left.mValue == right.mValue;
        }

        return false;
    }

    [[nodiscard]] friend auto operator!=(const Value &left, const Value &right) noexcept -> bool
    {
        return !(left == right);
    }

    acore::size_type mIndex = 0;
    acore::Variant mValue;
};

template<>
Value::operator std::string() const
{
    if (isTrivial())
    {
        std::string val(sizeof(mIndex), '\0');
        std::memcpy(&val[0], &mIndex, sizeof(mIndex));
        return val;
    }

    return mValue.value<std::string>();
}
}

#endif
