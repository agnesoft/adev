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

#ifndef ACORE_COMMANDLINEOPTION_HPP
#define ACORE_COMMANDLINEOPTION_HPP

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace acore
{
//! The CommandLineOption is a builder class that
//! provides fluent interface for configuration of
//! command line options and their matching.
//!
//! The building is started by instantiating the
//! CommandLineOption::Option class (usually via
//! CommandLine object). The available methods then
//! guide the configuration of the option allowing
//! many optional settings where appropriate.
//!
//! The option can be either named or positional.
//! The named options are required to have a long
//! name and a description. Positional arguments
//! require only a description. The description is
//! intentionally made mandatory. All options must
//! also be bound to a value.
//!
//! Optional parameters are:
//!
//! - required: an option must be matched otherwise an error is raised
//! - defaultValue: if an option is not matched set the bound value to this value
//! - shortName: (only for named options) is a single character alternative for a named option
//!
//! Furthermore it is possible to specify a repeated
//! option simply by binding the value to an std::vector
//! of a selected type. Such an option can be matched
//! multiple times and will append all matched values
//! to the vector. Repeated values can be required
//! or defaulted like a regular option.
//!
//! The requirements for the named options are:
//!
//! - long name: <tt> [a-zA-Z][a-zA-Z\\d]+ </tt> (a letter of either case followed by at least one or more letter(s) of either case or by digit(s) or combination of both)
//! - short name: <tt> [a-zA-Z] </tt> (a single alphabetic character, either lower or upper case)
//!
//! There is no restrictions placed on positional
//! arguments. The matching is done using the unified
//! syntax:
//!
//! - long name: always prefixed with double dash, <tt> e.g. --longName1 </tt>
//! - short name: always prefixed with single dash, <tt> e.g. -I </tt>
//!
//! The value corresponding to the option can be specified
//! in two ways:
//!
//! - equal sign, <tt> e.g. -I=/some/path/ </tt>
//! - as following value, <tt> e.g. --longName1 value </tt>
//!
//! The quoted values, both named or positional are
//! unquoted, e.g. "some value with spaces" will be
//! parsed and saved to the bound value as <tt>some value with spaces</tt>
//! without any quotes.
class CommandLineOption
{
public:
    class Option;

    //! A variant holding a default value to be bound
    //! if an option is not matched.
    using DefaultValue = std::variant<std::monostate, bool, std::int64_t, double, std::string, std::vector<std::int64_t>, std::vector<double>, std::vector<std::string>>;

    //! A variant holding a pointer to the bound value.
    using BoundValue = std::variant<std::monostate, bool *, std::int64_t *, double *, std::string *, std::vector<std::int64_t> *, std::vector<double> *, std::vector<std::string> *>;

    //! Returns the currently bound value.
    [[nodiscard]] auto boundValue() const noexcept -> const BoundValue &;

    //! Returns the type of the currently bound value
    //! as string to be displayed in the help. E.g.
    //! bool * -> "switch"
    //! std::int64_t -> "integer"
    //! std::vector<std::string> -> "string list"
    [[nodiscard]] auto boundValueAsString() const -> std::string;

    //! Sets the bound value to the default value.
    //! If either of the two was not set this
    //! method does nothing.
    auto defaultBoundValue() const -> void;

    //! Return the currently set default value.
    [[nodiscard]] auto defaultValue() const noexcept -> const DefaultValue &;

    //! Returns the currently set default value as
    //! string (e.g. as if printed to std::cout).
    [[nodiscard]] auto defaultValueAsString() const -> std::string;

    //! Returns the option's description.
    [[nodiscard]] auto description() const noexcept -> const std::string &;

    //! Returns true if there is a default value set.
    [[nodiscard]] auto isDefaulted() const noexcept -> bool;

    //! Returns true if this is a positional option.
    [[nodiscard]] auto isPositional() const noexcept -> bool;

    //! Returns true if the option can be repeated.
    //! I.e. it is bound to std::vector value.
    [[nodiscard]] auto isRepeated() const noexcept -> bool;

    //! Returns true if the option is required.
    [[nodiscard]] auto isRequired() const noexcept -> bool;

    //! Returns true of the option is a switch. I.e.
    //! it is bound to \c bool.
    [[nodiscard]] constexpr auto isSwitch() const noexcept -> bool
    {
        return std::holds_alternative<bool *>(boundValue());
    }

    //! Returns the option's long name. The string
    //! is empty for positional options.
    [[nodiscard]] auto longName() const noexcept -> const std::string &;

    //! Matches the option against \a argument and
    //! advances it accordingly. If the option was
    //! matched including the value this method
    //! returns true and the \a argument is advanced
    //! by 1. If the option was matched and the value
    //! was taken from the next argument the function
    //! returns true and the \a argument is advanced
    //! by 2. The \a end is used as a guard. If the
    //! option is not matched the method returns
    //! false and the \a argument remains unchanged.
    [[nodiscard]] auto match(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) const -> bool;

    //! Returns the name of the option. It is either
    //! the long name or "[positional]" for positional
    //! options.
    [[nodiscard]] auto name() const -> std::string;

    //! Returns the short name of the option. The
    //! value is 0 if the short name was not set
    //! or the option is positional.
    [[nodiscard]] auto shortName() const noexcept -> char;

private:
    class Base;
    class Defined;
    class Described;
    class Named;
    class Value;

    auto bindTo(CommandLineOption::BoundValue value) -> void;
    [[nodiscard]] auto extractLongName(const std::string &argument) const -> std::string;
    [[nodiscard]] auto extractName(const std::string &argument) const -> std::string;
    [[nodiscard]] auto extractShortName(const std::string &argument) const -> std::string;
    [[nodiscard]] auto extractValue(const std::string &argument) const -> std::string;
    [[nodiscard]] auto extractNamedValue(const std::string &argument) const -> std::string;
    [[nodiscard]] auto isLongName(const std::string &longName) const -> bool;
    [[nodiscard]] auto isQuoted(const std::string &value) const noexcept -> bool;
    [[nodiscard]] auto isShortName(unsigned char shortName) const noexcept -> bool;
    [[nodiscard]] auto matchOption(const std::string &name) const noexcept -> bool;
    auto setDefaultValue(CommandLineOption::DefaultValue value) noexcept -> void;
    auto setDescription(std::string description) noexcept -> void;
    auto setLongName(std::string name) -> void;
    auto setRequired() noexcept -> void;
    auto setShortName(char name) -> void;
    [[nodiscard]] auto setValue(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) const -> bool;
    [[nodiscard]] auto setValue(const std::string &value) const -> bool;
    [[nodiscard]] auto unquote(const std::string &value) const -> std::string;

    std::string mLongName;
    std::string mDescription;
    CommandLineOption::DefaultValue mDefaultValue;
    CommandLineOption::BoundValue mBoundValue;
    bool mRequired = false;
    char mShortName{};
};

class CommandLineOption::Base
{
public:
    explicit Base(CommandLineOption *option) noexcept;

protected:
    [[nodiscard]] auto option() const noexcept -> CommandLineOption *;

private:
    CommandLineOption *mOption = nullptr;
};

class CommandLineOption::Option : public CommandLineOption::Base
{
public:
    using Base::Base;

    [[nodiscard]] auto longName(std::string name) -> Named;
    [[nodiscard]] auto positional() noexcept -> Value;
};

class CommandLineOption::Defined : public CommandLineOption::Base
{
public:
    using Base::Base;

    [[nodiscard]] auto description(std::string desc) noexcept -> Described;
};

class CommandLineOption::Value : public CommandLineOption::Base
{
public:
    using Base::Base;

    [[nodiscard]] auto defaultValue(DefaultValue defaultValue) noexcept -> Defined;
    [[nodiscard]] auto description(std::string description) noexcept -> Described;
    [[nodiscard]] auto required() noexcept -> Defined;
};

class CommandLineOption::Named : public CommandLineOption::Value
{
public:
    using Value::Value;

    [[nodiscard]] auto shortName(char shortName) -> Value;
};

class CommandLineOption::Described : public CommandLineOption::Base
{
public:
    using Base::Base;

    auto bindTo(BoundValue value) -> const CommandLineOption &;
};
}

#endif
