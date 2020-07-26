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
class Value
{
    using TrivialValue = std::array<char, sizeof(acore::size_type)>;

public:
    Value() = default;

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

    explicit Value(const char *value) :
        Value(std::string_view{value})
    {
    }

    Value(const std::string &value) :
        Value(std::string_view{value})
    {
    }

    template<typename T>
    auto get() const -> T
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

    template<>
    auto get() const -> std::string_view
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

    template<>
    auto get() const -> std::string
    {
        return std::string{get<std::string_view>()};
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
}

#endif
