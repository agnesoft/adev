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

#include "pch.hpp"

#include <catch2/catch.hpp>

#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace cppcommandlineoptiontest
{
TEST_CASE("[acore::CommandLineOption]")
{
    REQUIRE(std::is_default_constructible_v<acore::CommandLineOption>);
    REQUIRE(std::is_copy_constructible_v<acore::CommandLineOption>);
    REQUIRE(std::is_nothrow_move_constructible_v<acore::CommandLineOption>);
    REQUIRE(std::is_copy_assignable_v<acore::CommandLineOption>);
    REQUIRE(std::is_nothrow_move_assignable_v<acore::CommandLineOption>);
    REQUIRE(std::is_nothrow_destructible_v<acore::CommandLineOption>);
}

TEST_CASE("CommandLineOption() [acore::CommandLineOption]")
{
    REQUIRE_NOTHROW(acore::CommandLineOption{});
}

TEST_CASE("CommandLineOption(const CommandLineOption &other) [acore::CommandLineOption]")
{
    std::string value;
    acore::CommandLineOption option;
    acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
    acore::CommandLineOption other{option};
    REQUIRE(option.longName() == std::string("longName1"));
    REQUIRE(other.longName() == std::string("longName1"));
}

TEST_CASE("CommandLineOption(CommandLineOption &&other) noexcept [acore::CommandLineOption]")
{
    std::string value;
    acore::CommandLineOption option;
    acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
    acore::CommandLineOption other{std::move(option)};
    REQUIRE(noexcept(acore::CommandLineOption{std::move(option)})); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    REQUIRE(other.longName() == std::string("longName1"));
}

TEST_CASE("bindTo(BoundValue value) -> const CommandLineOption & [acore::CommandLineOption]")
{
    acore::CommandLineOption option;
    std::string value;

    SECTION("[positional]")
    {
        REQUIRE_NOTHROW(acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value));
    }

    SECTION("[named]")
    {
        REQUIRE_NOTHROW(acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value));
    }

    SECTION("[defaulted]")
    {
        REQUIRE_NOTHROW(acore::CommandLineOption::Option{&option}.longName("longName1").defaultValue(std::string{"someValue"}).description("").bindTo(&value));
    }

    SECTION("[repeated]")
    {
        std::vector<double> val;
        REQUIRE_NOTHROW(acore::CommandLineOption::Option{&option}.longName("longName1").description("").bindTo(&val));
    }

    SECTION("[defaulted different type]")
    {
        REQUIRE_THROWS_AS(acore::CommandLineOption::Option{&option}.longName("longName1").defaultValue(std::int64_t{10}).description("").bindTo(&value), acore::Exception);
    }
}

TEST_CASE("boundValue() const -> BoundValue [acore::CommandLineOption]")
{
    std::string value;
    acore::CommandLineOption::BoundValue boundValue{&value};
    acore::CommandLineOption option;

    SECTION("[positional]")
    {
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValue() == boundValue);
        REQUIRE(value == ""); //NOLINT(readability-container-size-empty)
    }

    SECTION("[named]")
    {
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValue() == boundValue);
        REQUIRE(value == ""); //NOLINT(readability-container-size-empty)
    }

    SECTION("[defaulted]")
    {
        acore::CommandLineOption::Option{&option}.longName("longName1").defaultValue(std::string{"someValue"}).description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValue() == boundValue);
        option.defaultBoundValue();
        REQUIRE(value == "someValue");
    }

    SECTION("[repeated]")
    {
        std::vector<std::string> val;
        acore::CommandLineOption::BoundValue boundVal{&val};
        acore::CommandLineOption::Option{&option}.longName("longName1").description("").bindTo(boundVal);
        REQUIRE(std::as_const(option).boundValue() == boundVal);
        option.defaultBoundValue();
        REQUIRE(val == std::vector<std::string>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[defaulted repeated]")
    {
        std::vector<std::string> val;
        acore::CommandLineOption::BoundValue boundVal{&val};
        acore::CommandLineOption::Option{&option}.longName("longName1").defaultValue(std::vector<std::string>{"some", "value"}).description("").bindTo(boundVal);
        REQUIRE(std::as_const(option).boundValue() == boundVal);
        option.defaultBoundValue();
        REQUIRE(val == std::vector<std::string>{"some", "value"});
    }
}

TEST_CASE("boundValueAsString() const -> std::string [acore::CommandLineOption]")
{
    acore::CommandLineOption option;

    SECTION("[bool]")
    {
        bool value = false;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValueAsString() == "switch");
    }

    SECTION("[double]")
    {
        double value = 0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValueAsString() == "floating");
    }

    SECTION("[std::int64_t]")
    {
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValueAsString() == "integer");
    }

    SECTION("[std::string]")
    {
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValueAsString() == "string");
    }

    SECTION("[Vector<std::int64_t>]")
    {
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValueAsString() == "integer list");
    }

    SECTION("[Vector<double>]")
    {
        std::vector<double> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValueAsString() == "floating list");
    }

    SECTION("[Vector<std::string>]")
    {
        std::vector<std::string> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(std::as_const(option).boundValueAsString() == "string list");
    }

    SECTION("[undefined]")
    {
        acore::CommandLineOption other;
        REQUIRE(std::as_const(other).boundValueAsString() == "undefined");
    }
}

TEST_CASE("defaultBoundValue() const -> void [acore::CommandLineOption]")
{
    acore::CommandLineOption option;
    std::string value;

    SECTION("[defaulted]")
    {
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::string{"def value"}).description("").bindTo(&value);
        option.defaultBoundValue();
        REQUIRE(value == "def value");
    }

    SECTION("[non defaulted]")
    {
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        option.defaultBoundValue();
        REQUIRE(value == std::string{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[non bound]")
    {
        static_cast<void>(acore::CommandLineOption::Option{&option}.positional().defaultValue(std::string{"some value"}));
        REQUIRE_THROWS_AS(option.defaultBoundValue(), acore::Exception);
    }
}

TEST_CASE("defaultValue() const -> DefaultValue [acore::CommandLineOption]")
{
    acore::CommandLineOption option;
    std::string value;

    SECTION("[defaulted]")
    {
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::string{"option"}).description("").bindTo(&value);
        REQUIRE(std::as_const(option).defaultValue() == acore::CommandLineOption::DefaultValue{std::string{"option"}});
    }

    SECTION("[non defaulted]")
    {
        const acore::CommandLineOption other;
        REQUIRE(std::as_const(other).defaultValue() == acore::CommandLineOption::DefaultValue{});
    }
}

TEST_CASE("defaultValue(DefaultValue defaultValue) noexcept -> Defined [acore::CommandLineOption]")
{
    acore::CommandLineOption option;
    acore::CommandLineOption::DefaultValue defValue{std::string{"option"}};
    REQUIRE(noexcept(static_cast<void>(acore::CommandLineOption::Option{&option}.positional().defaultValue(std::move(defValue))))); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
}

TEST_CASE("defaultValueAsString() const -> std::string [acore::CommandLineOption]")
{
    acore::CommandLineOption option;

    SECTION("[bool]")
    {
        bool value = false;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(true).description("").bindTo(&value);
        REQUIRE(std::as_const(option).defaultValueAsString() == "true");
        acore::CommandLineOption::Option{&option}.positional().defaultValue(false).description("").bindTo(&value);
        REQUIRE(std::as_const(option).defaultValueAsString() == "false");
    }

    SECTION("[double]")
    {
        double value = 0;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(12.1).description("").bindTo(&value); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(std::as_const(option).defaultValueAsString() == "12.1");
    }

    SECTION("[std::int64_t]")
    {
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::int64_t{10}).description("").bindTo(&value); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(std::as_const(option).defaultValueAsString() == "10");
    }

    SECTION("[std::string]")
    {
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::string{"someValue"}).description("").bindTo(&value);
        REQUIRE(std::as_const(option).defaultValueAsString() == "someValue");
    }

    SECTION("[Vector<double>]")
    {
        std::vector<double> value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::vector<double>{-1.1, 13.3}).description("").bindTo(&value); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(std::as_const(option).defaultValueAsString() == "[-1.1, 13.3]");
    }

    SECTION("[Vector<std::int64_t>]")
    {
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::vector<std::int64_t>{10, 100}).description("").bindTo(&value); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(std::as_const(option).defaultValueAsString() == "[10, 100]");
    }

    SECTION("[Vector<std::string>]")
    {
        std::vector<std::string> value;
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::vector<std::string>{"some", "value", "and", "then", "some"}).description("").bindTo(&value);
        REQUIRE(std::as_const(option).defaultValueAsString() == "[some, value, and, then, some]");
    }
}

TEST_CASE("description() const noexcept -> const std::string & [acore::CommandLineOption]")
{
    std::int64_t value = 0;
    acore::CommandLineOption option;
    acore::CommandLineOption::Option{&option}.positional().description("My fancty description").bindTo(&value);
    REQUIRE(noexcept(std::as_const(option).description()));
    REQUIRE(std::as_const(option).description() == "My fancty description");
}

TEST_CASE("description(std::string description) noexcept -> Described [acore::CommandLineOption]")
{
    acore::CommandLineOption option;
    std::string desc = "My fancty description";
    REQUIRE(noexcept(static_cast<void>(acore::CommandLineOption::Option{&option}.positional().description(std::move(desc))))); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
}

TEST_CASE("isDefaulted() const noexcept -> bool [acore::CommandLineOption]")
{
    std::int64_t value = 0;
    acore::CommandLineOption option;

    SECTION("[non defaulted]")
    {
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(noexcept(std::as_const(option).isDefaulted()));
        REQUIRE_FALSE(std::as_const(option).isDefaulted());
    }

    SECTION("[defaulted]")
    {
        acore::CommandLineOption::Option{&option}.positional().defaultValue(std::int64_t{10}).description("").bindTo(&value); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(std::as_const(option).isDefaulted());
    }
}

TEST_CASE("isPositional() const noexcept -> bool [acore::CommandLineOption]")
{
    std::int64_t value = 0;
    acore::CommandLineOption option;

    SECTION("[positional]")
    {
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(noexcept(std::as_const(option).isPositional()));
        REQUIRE(std::as_const(option).isPositional());
    }

    SECTION("[named]")
    {
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        REQUIRE_FALSE(std::as_const(option).isPositional());
    }
}

TEST_CASE("isRepeaated() const noexcept -> bool [acore::CommandLineOption]")
{
    acore::CommandLineOption option;

    SECTION("[repeated]")
    {
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        REQUIRE(noexcept(option.isRepeated()));
        REQUIRE(option.isRepeated());
    }

    SECTION("[non repeated]")
    {
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        REQUIRE_FALSE(option.isRepeated());
    }
}

TEST_CASE("isRequired() const noexcept -> bool [acore::CommandLineOption]")
{
    std::int64_t value = 0;
    acore::CommandLineOption option;

    SECTION("[positional required]")
    {
        acore::CommandLineOption::Option{&option}.positional().required().description("").bindTo(&value);
        REQUIRE(noexcept(std::as_const(option).isRequired()));
        REQUIRE(std::as_const(option).isRequired());
    }

    SECTION("[named required]")
    {
        acore::CommandLineOption::Option{&option}.longName("longName1").required().description("").bindTo(&value);
        REQUIRE(std::as_const(option).isRequired());
    }

    SECTION("[non required]")
    {
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE_FALSE(std::as_const(option).isRequired());
    }
}

TEST_CASE("isSwitch()) const noexcept -> bool [acore::CommandLineOption]")
{
    acore::CommandLineOption option;

    SECTION("[switch]")
    {
        bool value = false;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(std::as_const(option).isSwitch());
    }

    SECTION("[non switch]")
    {
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        REQUIRE_FALSE(std::as_const(option).isSwitch());
    }
}

TEST_CASE("longName() const noexcept -> const std::string & [acore::CommandLineOption]")
{
    std::string value;
    acore::CommandLineOption option;

    SECTION("[positional]")
    {
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(noexcept(std::as_const(option).longName()));
        REQUIRE(std::as_const(option).longName() == std::string{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[named]")
    {
        acore::CommandLineOption::Option{&option}.longName("longName1").description("").bindTo(&value);
        REQUIRE(option.longName() == "longName1");
    }
}

TEST_CASE("longName(std::string name) -> Value [acore::CommandLineOption]")
{
    acore::CommandLineOption option;

    SECTION("[valid]")
    {
        REQUIRE_NOTHROW(acore::CommandLineOption::Option{&option}.longName("longName1"));
    }

    SECTION("[invalid]")
    {
        REQUIRE_THROWS_AS(acore::CommandLineOption::Option{&option}.longName("1longName"), acore::Exception);
        REQUIRE_THROWS_AS(acore::CommandLineOption::Option{&option}.longName("l"), acore::Exception);
        REQUIRE_THROWS_AS(acore::CommandLineOption::Option{&option}.longName("1ongName#"), acore::Exception);
    }
}

TEST_CASE("match(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) const -> bool [acore::CommandLineOption]")
{
    acore::CommandLineOption option;

    SECTION("[unmatched]")
    {
        std::string value;
        acore::CommandLineOption::Option{&option}.longName("longName1").description("").bindTo(&value);
        std::vector<std::string> arguments{"--longName", "value"};
        auto arg = arguments.cbegin();
        REQUIRE_FALSE(option.match(&arg, arguments.cend()));
        REQUIRE(arg == arguments.cbegin());
        REQUIRE(value == std::string{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[--longName value]")
    {
        std::string value;
        acore::CommandLineOption::Option{&option}.longName("longName").description("").bindTo(&value);
        std::vector<std::string> arguments{"--longName", "\"quoted value and then some\""};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(arg == arguments.cend());
        REQUIRE(value == "quoted value and then some");
    }

    SECTION("[--longName=value]")
    {
        std::string value;
        acore::CommandLineOption::Option{&option}.longName("longName").shortName('l').description("").bindTo(&value);
        std::vector<std::string> arguments{"--longName=\"a value\""};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value == "a value");
    }

    SECTION("[positional bool]")
    {
        bool value = false;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"positional"};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value);
    }

    SECTION("[positional value]")
    {
        std::string value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"value"};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value == "value");
    }

    SECTION("[int64_t]")
    {
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"10"};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value == 10);
    }

    SECTION("[negative int]")
    {
        std::int64_t value = 0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"-10"};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value == -10);
    }

    SECTION("[double]")
    {
        double value = 0.0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"15.5"};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value == 15.5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }

    SECTION("[negative double]")
    {
        double value = 0.0;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"-15.5"};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value == -15.5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }

    SECTION("[repeated std::int64_t]")
    {
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.longName("repeated").shortName('r').description("").bindTo(&value);
        std::vector<std::string> arguments{"--repeated=-10"};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value == std::vector<std::int64_t>{-10});
    }

    SECTION("[repeated double]")
    {
        std::vector<double> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"-15.5", "20.77", "-1.0"};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value == std::vector<double>{-15.5, 20.77, -1.0}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    }

    SECTION("[repeated std::string]")
    {
        std::vector<std::string> value;
        acore::CommandLineOption::Option{&option}.longName("repeated").shortName('r').description("").bindTo(&value);
        std::vector<std::string> arguments{"-r", "someValue", "-r=\"my Val\""};
        auto arg = arguments.cbegin();
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(option.match(&arg, arguments.cend()));
        REQUIRE(value == std::vector<std::string>{"someValue", "my Val"});
    }

    SECTION("[type mismatch]")
    {
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.longName("repeated").shortName('r').description("").bindTo(&value);
        std::vector<std::string> arguments{"--repeated=hello"};
        auto arg = arguments.cbegin();
        REQUIRE_THROWS_AS(option.match(&arg, arguments.cend()), acore::Exception);
    }

    SECTION("[type mismatch positional]")
    {
        std::vector<std::int64_t> value;
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        std::vector<std::string> arguments{"--repeated=hello"};
        auto arg = arguments.cbegin();
        REQUIRE_FALSE(option.match(&arg, arguments.cend()));
        REQUIRE(value == std::vector<std::int64_t>{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[non bound value]")
    {
        static_cast<void>(acore::CommandLineOption::Option{&option}.positional().description(""));
        std::vector<std::string> arguments{"value"};
        auto arg = arguments.cbegin();
        REQUIRE_THROWS_AS(option.match(&arg, arguments.cend()), acore::Exception);
    }
}

TEST_CASE("name() const -> std::string [acore::CommandLineOption]")
{
    double value = 0.0;
    acore::CommandLineOption option;

    SECTION("[positional]")
    {
        acore::CommandLineOption::Option{&option}.positional().description("").bindTo(&value);
        REQUIRE(option.name() == "[positional]");
    }

    SECTION("[named]")
    {
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
        REQUIRE(option.name() == "longName1");
    }
}

TEST_CASE("required() noexcept -> Defined [acore::CommandLineOption]")
{
    acore::CommandLineOption option;
    REQUIRE(noexcept(static_cast<void>(acore::CommandLineOption::Option{&option}.positional().required())));
}

TEST_CASE("shortName() const noexcept -> char [acore::CommandLineOption]")
{
    std::string value;
    acore::CommandLineOption option;

    SECTION("[valid]")
    {
        acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').required().description("").bindTo(&value);
        REQUIRE(option.shortName() == 'l');
    }

    SECTION("[invalid]")
    {
        REQUIRE_THROWS_AS(acore::CommandLineOption::Option{&option}.longName("longName1").shortName('1'), acore::Exception);
        REQUIRE_THROWS_AS(acore::CommandLineOption::Option{&option}.longName("longName1").shortName('\0'), acore::Exception);
    }
}

TEST_CASE("shortName() -> Value [acore::CommandLineOption]")
{
    std::string value;
    acore::CommandLineOption option;
    REQUIRE_NOTHROW(acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value));
}

TEST_CASE("operator=(const CommandLineOption &other) -> CommandLineOption & [acore::CommandLineOption]")
{
    std::string value;
    acore::CommandLineOption option;
    acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
    acore::CommandLineOption other;
    other = {option};
    REQUIRE(option.longName() == std::string("longName1"));
    REQUIRE(other.longName() == std::string("longName1"));
}

TEST_CASE("operator=(CommandLineOption &&other) noexcept -> CommandLineOption & [acore::CommandLineOption]")
{
    std::string value;
    acore::CommandLineOption option;
    acore::CommandLineOption::Option{&option}.longName("longName1").shortName('l').description("").bindTo(&value);
    acore::CommandLineOption other;
    other = {std::move(option)};
    REQUIRE(noexcept(acore::CommandLineOption{std::move(option)})); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    REQUIRE(other.longName() == std::string("longName1"));
}
}
