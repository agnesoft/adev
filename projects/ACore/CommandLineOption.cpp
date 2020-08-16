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

#include "CommandLineOption.hpp"
#include "Exception.hpp"

#include <algorithm>
#include <exception>
#include <sstream>
#include <type_traits>

namespace acore
{
template<typename T>
auto operator<<(std::ostringstream &stream, const std::vector<T> &vector) -> std::ostringstream &
{
    stream << "[";

    for (const T &value : vector)
    {
        stream << value;

        if (&vector.back() != &value)
        {
            stream << ", ";
        }
    }

    stream << "]";
    return stream;
}

auto CommandLineOption::bindTo(CommandLineOption::BoundValue value) -> void
{
    std::visit([&](auto &&boundVal) {
        using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundVal)>>;

        std::visit([&](auto &&defVal) {
            using DefaultT = std::decay_t<decltype(defVal)>;

            if constexpr (!std::is_same_v<DefaultT, std::monostate> && !std::is_same_v<BoundT, DefaultT>)
            {
                throw Exception{} << "The option " << name() << " default value is set with incompatible type (" << typeid(DefaultT).name() << ") to the one it is being bound to (" << typeid(BoundT).name() << ')';
            }
        },
                   defaultValue());
    },
               value);

    mBoundValue = value;
}

auto CommandLineOption::boundValue() const noexcept -> const BoundValue &
{
    return mBoundValue;
}

auto CommandLineOption::boundValueAsString() const -> std::string
{
    std::stringstream stream;

    std::visit([&](auto &&value) {
        using BoundT = std::remove_pointer_t<std::decay_t<decltype(value)>>;

        if constexpr (std::is_same_v<BoundT, bool>)
        {
            stream << "switch";
        }
        else if constexpr (std::is_same_v<BoundT, std::int64_t>)
        {
            stream << "integer";
        }
        else if constexpr (std::is_same_v<BoundT, double>)
        {
            stream << "floating";
        }
        else if constexpr (std::is_same_v<BoundT, std::string>)
        {
            stream << "string";
        }
        else if constexpr (std::is_same_v<BoundT, std::vector<std::int64_t>>)
        {
            stream << "integer list";
        }
        else if constexpr (std::is_same_v<BoundT, std::vector<double>>)
        {
            stream << "floating list";
        }
        else if constexpr (std::is_same_v<BoundT, std::vector<std::string>>)
        {
            stream << "string list";
        }
        else
        {
            stream << "undefined";
        }
    },
               boundValue());

    return stream.str();
}

auto CommandLineOption::defaultBoundValue() const -> void
{
    if (isDefaulted())
    {
        std::visit([&](auto &&boundVal) {
            using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundVal)>>;

            std::visit([&](auto &&defVal) {
                using DefaultT = std::decay_t<decltype(defVal)>;

                if constexpr (!std::is_same_v<BoundT, std::monostate>)
                {
                    if constexpr (std::is_same_v<BoundT, DefaultT>)
                    {
                        *boundVal = defVal;
                    }
                }
                else
                {
                    throw Exception{} << "The option " << name() << " is missing a bound value.";
                }
            },
                       defaultValue());
        },
                   boundValue());
    }
}

auto CommandLineOption::defaultValue() const noexcept -> const DefaultValue &
{
    return mDefaultValue;
}

auto CommandLineOption::defaultValueAsString() const -> std::string
{
    std::ostringstream stream;

    std::visit([&](auto &&value) {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, bool>)
        {
            stream << (value ? "true" : "false");
        }
        else if constexpr (!std::is_same_v<T, std::monostate>)
        {
            stream << value;
        }
    },
               defaultValue());

    return stream.str();
}

auto CommandLineOption::description() const noexcept -> const std::string &
{
    return mDescription;
}

auto CommandLineOption::isDefaulted() const noexcept -> bool
{
    return !std::holds_alternative<std::monostate>(defaultValue());
}

auto CommandLineOption::isPositional() const noexcept -> bool
{
    return mLongName.empty();
}

auto CommandLineOption::isRepeated() const noexcept -> bool
{
    return std::holds_alternative<std::vector<std::int64_t> *>(boundValue())
        || std::holds_alternative<std::vector<double> *>(boundValue())
        || std::holds_alternative<std::vector<std::string> *>(boundValue());
}

auto CommandLineOption::isRequired() const noexcept -> bool
{
    return mRequired;
}

auto CommandLineOption::longName() const noexcept -> const std::string &
{
    return mLongName;
}

auto CommandLineOption::match(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) const -> bool
{
    return matchOption(extractName(**argument)) && setValue(argument, end);
}

auto CommandLineOption::name() const -> std::string
{
    if (longName().empty())
    {
        return "[positional]";
    }

    return longName();
}

auto CommandLineOption::shortName() const noexcept -> char
{
    return mShortName;
}

CommandLineOption::Base::Base(CommandLineOption *option) noexcept :
    mOption{option}
{
}

auto CommandLineOption::Base::option() const noexcept -> CommandLineOption *
{
    return mOption;
}

auto CommandLineOption::setDefaultValue(CommandLineOption::DefaultValue value) noexcept -> void
{
    mDefaultValue = std::move(value);
}

auto CommandLineOption::setDescription(std::string description) noexcept -> void
{
    mDescription = std::move(description);
}

auto CommandLineOption::setLongName(std::string name) -> void
{
    if (!isLongName(name))
    {
        throw Exception{} << "Long name '" << name << "' is not a valid name ([a-zA-Z][a-zA-Z\\d]+).";
    }

    mLongName = std::move(name);
}

auto CommandLineOption::setRequired() noexcept -> void
{
    mRequired = true;
}

auto CommandLineOption::setShortName(char name) -> void
{
    if (!isShortName(name))
    {
        throw Exception{} << "Short name '" << name << "' is not a valid name ([a-zA-Z]).";
    }

    mShortName = name;
}

auto CommandLineOption::extractLongName(const std::string &argument) -> std::string
{
    if (argument.size() > 3
        && argument[0] == '-'
        && argument[1] == '-')
    {
        const auto pos = argument.find('=', 3);
        std::string name = argument.substr(2, pos - 2);

        if (isLongName(name))
        {
            return name;
        }
    }

    return {};
}

auto CommandLineOption::extractName(const std::string &argument) -> std::string
{
    std::string optionName = extractShortName(argument);

    if (optionName.empty())
    {
        optionName = extractLongName(argument);
    }

    return optionName;
}

auto CommandLineOption::extractNamedValue(const std::string &argument) -> std::string
{
    const size_t pos = argument.find('=');

    if (pos != std::string::npos)
    {
        return argument.substr(pos + 1);
    }

    return {};
}

auto CommandLineOption::extractShortName(const std::string &argument) -> std::string
{
    if (argument.size() >= 2
        && argument[0] == '-'
        && isShortName(static_cast<unsigned char>(argument[1]))
        && (argument.size() == 2 || argument[2] == '='))
    {
        return argument.substr(1, 1);
    }

    return {};
}

auto CommandLineOption::extractValue(const std::string &argument) const -> std::string
{
    if (isPositional())
    {
        return argument;
    }

    return extractNamedValue(argument);
}

auto CommandLineOption::isLongName(const std::string &longName) -> bool
{
    const auto isNotAlphanumeric = [](char c) {
        return std::isalnum(c) == 0;
    };
    return longName.size() > 1 && isShortName(longName[0]) && std::find_if(++longName.begin() + 1, longName.end(), isNotAlphanumeric) == longName.end();
}

auto CommandLineOption::isQuoted(const std::string &value) noexcept -> bool
{
    return value.size() >= 2 && value.front() == '"' && value.back() == '"';
}

auto CommandLineOption::isShortName(unsigned char shortName) noexcept -> bool
{
    return std::isalpha(shortName) != 0;
}

auto CommandLineOption::matchOption(const std::string &name) const noexcept -> bool
{
    return name == longName()
        || (name.size() == 1 && name[0] == shortName())
        || (name.empty() && isPositional());
}

auto CommandLineOption::setValue(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) const -> bool
{
    std::string value = extractValue(**argument);

    if (value.empty() && !isSwitch())
    {
        if (++(*argument) == end)
        {
            throw Exception{} << "Missing value for option '" << longName() << '\'';
        }

        value = **argument;
    }

    if (setValue(value))
    {
        ++(*argument);
        return true;
    }

    return false;
}

auto CommandLineOption::setValue(const std::string &value) const -> bool
{
    try
    {
        std::visit([&](auto &&boundValue) {
            using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;

            if constexpr (std::is_same_v<BoundT, std::monostate>)
            {
                throw Exception{} << "Bind value undefined for option '" << name() << '\'';
            }
            else if constexpr (std::is_same_v<BoundT, bool>)
            {
                *boundValue = true;
            }
            else if constexpr (std::is_same_v<BoundT, double>)
            {
                *boundValue = std::stod(value);
            }
            else if constexpr (std::is_same_v<BoundT, std::int64_t>)
            {
                *boundValue = std::stoll(value);
            }
            else if constexpr (std::is_same_v<BoundT, std::string>)
            {
                *boundValue = unquote(value);
            }
            else if constexpr (std::is_same_v<BoundT, std::vector<std::int64_t>>)
            {
                (*boundValue).emplace_back(std::stoll(value));
            }
            else if constexpr (std::is_same_v<BoundT, std::vector<double>>)
            {
                (*boundValue).emplace_back(std::stod(value));
            }
            else if constexpr (std::is_same_v<BoundT, std::vector<std::string>>)
            {
                (*boundValue).emplace_back(unquote(value));
            }
        },
                   boundValue());

        return true;
    }
    catch ([[maybe_unused]] std::exception &e)
    {
        if (!isPositional())
        {
            std::visit([&](auto &&boundValue) {
                using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;

                throw Exception{} << "Failed to set value of type '" << typeid(BoundT).name() << "' for option '" << name() << "' from value '" << value << '\'';
            },
                       boundValue());
        }
    }

    return false;
}

auto CommandLineOption::unquote(const std::string &value) -> std::string
{
    if (isQuoted(value))
    {
        return std::string{value.begin() + 1, value.end() - 1};
    }

    return value;
}

auto CommandLineOption::Defined::description(std::string description) noexcept -> Described
{
    option()->setDescription(std::move(description));
    return Described{option()};
}

auto CommandLineOption::Named::shortName(char shortName) -> Value
{
    option()->setShortName(shortName);
    return Value{option()};
}

auto CommandLineOption::Option::longName(std::string name) -> Named
{
    option()->setLongName(std::move(name));
    return Named{option()};
}

auto CommandLineOption::Option::positional() noexcept -> Value
{
    return Value{option()};
}

auto CommandLineOption::Value::defaultValue(DefaultValue defaultValue) noexcept -> Defined
{
    option()->setDefaultValue(std::move(defaultValue));
    return Defined{option()};
}

auto CommandLineOption::Value::description(std::string description) noexcept -> Described
{
    option()->setDescription(std::move(description));
    return Described{option()};
}

auto CommandLineOption::Value::required() noexcept -> Defined
{
    option()->setRequired();
    return Defined{option()};
}

auto CommandLineOption::Described::bindTo(BoundValue value) -> const CommandLineOption &
{
    option()->bindTo(value);
    return *option();
}
}
