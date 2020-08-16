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

#include <iostream>

namespace commandlinetest
{
TEST_CASE("[acore::CommandLine]")
{
    REQUIRE(std::is_default_constructible_v<acore::CommandLine>);
    REQUIRE(std::is_copy_constructible_v<acore::CommandLine>);
    REQUIRE(std::is_nothrow_move_constructible_v<acore::CommandLine>);
    REQUIRE(std::is_copy_assignable_v<acore::CommandLine>);
    REQUIRE(std::is_nothrow_move_assignable_v<acore::CommandLine>);
    REQUIRE(std::is_nothrow_destructible_v<acore::CommandLine>);
}

TEST_CASE("CommandLine() [acore::CommandLine]")
{
    REQUIRE_NOTHROW(acore::CommandLine{});
}

TEST_CASE("CommandLine(const CommandLine &other) [acore::CommandLine]")
{
    std::int64_t value = 0;
    acore::CommandLine commandLine;
    commandLine.option().positional().description("").bindTo(&value);
    acore::CommandLine other{commandLine};
    other.parse(2, std::vector<const char *>{"./app.exe", "10"}.data());
    REQUIRE(value == 10);
    commandLine.parse(2, std::vector<const char *>{"./diff", "100"}.data());
    REQUIRE(value == 100);
}

TEST_CASE("CommandLine(CommandLine &&other) [acore::CommandLine]")
{
    std::int64_t value = 0;
    acore::CommandLine commandLine;
    commandLine.option().positional().description("").bindTo(&value);
    acore::CommandLine other{std::move(commandLine)};
    other.parse(2, std::vector<const char *>{"./app.exe", "10"}.data());
    REQUIRE(value == 10);
}

TEST_CASE("applicationName() const noexcept -> const std::string & [acore::CommandLine]")
{
    acore::CommandLine commandLine;
    commandLine.parse(1, std::vector<const char *>{"./app.exe"}.data());
    REQUIRE(commandLine.applicationName() == "app");
    commandLine.parse(1, std::vector<const char *>{"./app.a"}.data());
    REQUIRE(commandLine.applicationName() == "app");
    commandLine.parse(1, std::vector<const char *>{"./app"}.data());
    REQUIRE(commandLine.applicationName() == "app");
    commandLine.parse(1, std::vector<const char *>{"app.exe"}.data());
    REQUIRE(commandLine.applicationName() == "app");
    commandLine.parse(1, std::vector<const char *>{"app"}.data());
    REQUIRE(commandLine.applicationName() == "app");
}

TEST_CASE("command() const noexcept -> const std::string & [acore::CommandLine]")
{
    acore::CommandLine commandLine;
    commandLine.parse(1, std::vector<const char *>{"./app.exe"}.data());
    REQUIRE(commandLine.command() == "./app.exe");
    commandLine.parse(1, std::vector<const char *>{"./app.a"}.data());
    REQUIRE(commandLine.command() == "./app.a");
    commandLine.parse(1, std::vector<const char *>{"./app"}.data());
    REQUIRE(commandLine.command() == "./app");
    commandLine.parse(1, std::vector<const char *>{"app.exe"}.data());
    REQUIRE(commandLine.command() == "app.exe");
    commandLine.parse(1, std::vector<const char *>{"app"}.data());
    REQUIRE(commandLine.command() == "app");
}

TEST_CASE("disableHelp() noexcept -> void [acore::CommandLine]")
{
    acore::CommandLine commandLine;
    commandLine.disableHelp();
    REQUIRE_FALSE(commandLine.helpEnabled());
}

TEST_CASE("enableHelp() noexcept -> void [acore::CommandLine]")
{
    acore::CommandLine commandLine;
    commandLine.disableHelp();
    REQUIRE_FALSE(commandLine.helpEnabled());
    commandLine.enableHelp();
    REQUIRE(commandLine.helpEnabled());
}

TEST_CASE("helpDisplayed() const noexcept -> bool [acore::CommandLine]")
{
    std::stringstream stream;
    acore::CommandLine commandLine;
    commandLine.setPrintStream(&stream);
    REQUIRE_FALSE(commandLine.helpDisplayed());
    commandLine.parse(2, std::vector<const char *>{"app", "-?"}.data());
    REQUIRE(commandLine.helpDisplayed());
}

TEST_CASE("helpEnabled() const noexcept -> bool [acore::CommandLine]")
{
    acore::CommandLine commandLine;
    REQUIRE(commandLine.helpEnabled());
}

TEST_CASE("option() -> CommandLineOption & [acore::CommandLine]")
{
    acore::CommandLine commandLine;
    REQUIRE_NOTHROW(static_cast<void>(commandLine.option()));
}

TEST_CASE("parse(int argc, char * const *argv) -> void -> CommandLineOption & [acore::CommandLine]")
{
    std::stringstream stream;
    acore::CommandLine commandLine;
    commandLine.disableHelp();

    SECTION("[help]")
    {
        bool b = false;
        std::int64_t i = 10; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        std::string str;
        double d{};

        commandLine.enableHelp();
        commandLine.setPrintStream(&stream);
        commandLine.option().longName("longName1").shortName('l').description("Some long name argument.").bindTo(&b);
        commandLine.option().longName("another").defaultValue(10.0).description("Another argument description.").bindTo(&d); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        commandLine.option().positional().required().description("Positional argument description.").bindTo(&str);
        commandLine.option().longName("output").shortName('o').description("Output parameter.").bindTo(&i);
        commandLine.parse(2, std::vector<const char *>{"app", "-?"}.data());

        const std::string expected = "Usage:\n"
                                     "    app [options]\n"
                                     "Syntax:\n"
                                     "  switch       --switch, -s\n"
                                     "  named        --name=value, --n=value, --name value, --n value\n"
                                     "  positional   value\n"
                                     "Options:\n"
                                     "    -?                  [switch]          Prints this help.\n"
                                     "  Required:\n"
                                     "    [positional]        [string]          Positional argument description.\n"
                                     "  Optional:\n"
                                     "    -l, --longName1     [switch]          Some long name argument.\n"
                                     "    --another           [floating=10]     Another argument description.\n"
                                     "    -o, --output        [integer]         Output parameter.\n"
                                     "  Positional:\n"
                                     "    [positional]        [string]          Positional argument description.\n";

        REQUIRE(stream.str() == expected);
    }

    SECTION("[default help]")
    {
        commandLine.enableHelp();
        std::cout.setstate(std::ios_base::failbit);
        REQUIRE_NOTHROW(commandLine.parse(2, std::vector<const char *>{"app", "-?"}.data()));
        std::cout.clear();
    }

    SECTION("[valid]")
    {
        commandLine.setPrintStream(&stream);

        SECTION("[single positional option]")
        {
            std::string value;
            commandLine.option().positional().description("").bindTo(&value);
            REQUIRE_NOTHROW(commandLine.parse(2, std::vector<const char *>{"./app.exe", "value"}.data()));
            REQUIRE(value == "value");
        }

        SECTION("[multiple positional options]")
        {
            std::string sValue;
            std::int64_t iValue = 0;
            double dValue = 0.0;
            commandLine.option().positional().description("").bindTo(&iValue);
            commandLine.option().positional().description("").bindTo(&dValue);
            commandLine.option().positional().description("").bindTo(&sValue);
            REQUIRE_NOTHROW(commandLine.parse(4, std::vector<const char *>{"./app", "value", "-10", "5.5"}.data()));
            REQUIRE(sValue == "value");
            REQUIRE(iValue == -10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(dValue == 5.5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("[single long name option]")
        {
            bool value = false;
            commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
            REQUIRE_NOTHROW(commandLine.parse(2, std::vector<const char *>{"./app", "--value"}.data()));
            REQUIRE(value);
        }

        SECTION("[multiple long name options]")
        {
            bool value = false;
            std::string option;
            std::int64_t another = 0;
            commandLine.option().longName("value").description("").bindTo(&value);
            commandLine.option().longName("option").description("").bindTo(&option);
            commandLine.option().longName("yetanother").shortName('y').description("").bindTo(&another);
            REQUIRE_NOTHROW(commandLine.parse(5, std::vector<const char *>{"./app", "--value", "--option=file", "--yetanother", "10"}.data()));
            REQUIRE(value);
            REQUIRE(option == "file");
            REQUIRE(another == 10);
        }

        SECTION("[single short name option]")
        {
            bool value = false;
            commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
            REQUIRE_NOTHROW(commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data()));
            REQUIRE(value);
        }

        SECTION("[multiple short name options]")
        {
            bool value = false;
            std::string option;
            std::int64_t another = 0;
            commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
            commandLine.option().longName("option").shortName('o').description("").bindTo(&option);
            commandLine.option().longName("yetanother").shortName('y').description("").bindTo(&another);
            REQUIRE_NOTHROW(commandLine.parse(5, std::vector<const char *>{"./app", "-v", "-o=file", "-y", "10"}.data()));
            REQUIRE(value);
            REQUIRE(option == "file");
            REQUIRE(another == 10);
        }

        SECTION("[mixed options]")
        {
            bool value = false;
            std::string option;
            std::int64_t another = 0;
            std::string positional;
            commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
            commandLine.option().longName("option").shortName('o').description("").bindTo(&option);
            commandLine.option().longName("yetanother").description("").bindTo(&another);
            commandLine.option().positional().description("").bindTo(&positional);
            REQUIRE_NOTHROW(commandLine.parse(6, std::vector<const char *>{"./app", "-v", "-o=file", "--yetanother", "10", "somefile"}.data()));
            REQUIRE(value);
            REQUIRE(option == "file");
            REQUIRE(another == 10);
            REQUIRE(positional == "somefile");
        }

        SECTION("[repeated named options]")
        {
            std::vector<double> values;
            commandLine.option().longName("value").shortName('v').description("").bindTo(&values);
            REQUIRE_NOTHROW(commandLine.parse(7, std::vector<const char *>{"./app", "-v", "33.3", "--value=51.61234", "-v=9.999", "-v", "19.19"}.data()));
            REQUIRE(values == std::vector{33.3, 51.61234, 9.999, 19.19}); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        }

        SECTION("[repeated positional options]")
        {
            std::vector<std::string> values;
            commandLine.option().positional().defaultValue(std::vector<std::string>{"input"}).description("").bindTo(&values);
            REQUIRE_NOTHROW(commandLine.parse(4, std::vector<const char *>{"./app", "input1", "input2", "input3"}.data()));
            REQUIRE(values == std::vector<std::string>{"input1", "input2", "input3"});
        }

        SECTION("mixed single, missing, defaulted and repeated options")
        {
            std::vector<int64_t> parameters;
            bool flag = true;
            std::string output;
            std::vector<std::string> paths;
            commandLine.option().longName("param").shortName('p').description("").bindTo(&parameters);
            commandLine.option().longName("flag").shortName('f').description("").bindTo(&flag);
            commandLine.option().positional().required().description("").bindTo(&output);
            commandLine.option().longName("include").shortName('I').defaultValue(std::vector<std::string>{"\"c:/path/with spaces/", "//"}).description("").bindTo(&paths);
            REQUIRE_NOTHROW(commandLine.parse(6, std::vector<const char *>{"a", "-p=1", "-p=2", "--param", "100", "output_option"}.data()));
            REQUIRE(parameters == std::vector<std::int64_t>{1, 2, 100});
            REQUIRE(flag);
            REQUIRE(output == "output_option");
            REQUIRE(paths == std::vector<std::string>{"\"c:/path/with spaces/", "//"});
        }
    }

    SECTION("[invalid]")
    {
        commandLine.setPrintStream(&stream);

        SECTION("[no arguments]")
        {
            REQUIRE_THROWS_AS(commandLine.parse(0, std::vector<const char *>{"./app", "-v"}.data()), acore::Exception);
        }

        SECTION("[unmatched argument]")
        {
            REQUIRE_THROWS_AS(commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data()), acore::Exception);
        }

        SECTION("[unmatched required option]")
        {
            bool value = false;
            double required = 0.0;
            commandLine.option().longName("longName").shortName('v').description("").bindTo(&value);
            commandLine.option().positional().required().description("").bindTo(&required);
            REQUIRE_THROWS_AS(commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data()), acore::Exception);
        }

        SECTION("[type mismatch]")
        {
            std::int64_t value = 0;
            commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
            REQUIRE_THROWS_AS(commandLine.parse(2, std::vector<const char *>{"./app", "-v=hello"}.data()), acore::Exception);
        }

        SECTION("[missing value]")
        {
            std::int64_t value = 0;
            commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
            REQUIRE_THROWS_AS(commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data()), acore::Exception);
        }

        SECTION("[required unmatched repeated value]")
        {
            std::vector<std::string> value;
            commandLine.option().positional().required().description("").bindTo(&value);
            REQUIRE_THROWS_AS(commandLine.parse(1, std::vector<const char *>{"app"}.data()), acore::Exception);
        }

        SECTION("[parsing error]")
        {
            commandLine.enableHelp();
            std::int64_t value = 0;
            commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
            REQUIRE_THROWS_AS(commandLine.parse(2, std::vector<const char *>{"./app", "-v=hello"}.data()), acore::Exception);
            const std::string expected = "ERROR parsing command line arguments: Failed to set value of type '"
                + std::string{typeid(std::int64_t).name()}
                + "' for option 'value' from value 'hello'\n"
                  "Use -? to list the command line options.\n";
            REQUIRE(stream.str() == expected);
        }
    }
}

TEST_CASE("setPrintStream(std::ostream *stream) -> void [acore::CommandLine]")
{
    std::stringstream stream;
    acore::CommandLine commandLine;
    commandLine.setPrintStream(&stream);
    commandLine.parse(2, std::vector<const char *>{"app", "-?"}.data());
    REQUIRE_FALSE(stream.str() == std::string{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("operator=(const CommandLine &other) -> CommandLine & [acore::CommandLine]")
{
    std::int64_t value = 0;
    acore::CommandLine commandLine;
    commandLine.option().positional().description("").bindTo(&value);
    acore::CommandLine other;
    other = commandLine;
    other.parse(2, std::vector<const char *>{"./app.exe", "10"}.data());
    REQUIRE(value == 10);
    commandLine.parse(2, std::vector<const char *>{"./diff", "100"}.data());
    REQUIRE(value == 100);
}

TEST_CASE("operator=(const CommandLine &other) noexcept -> CommandLine & [acore::CommandLine]")
{
    std::int64_t value = 0;
    acore::CommandLine commandLine;
    commandLine.option().positional().description("").bindTo(&value);
    acore::CommandLine other;
    other = std::move(commandLine);
    other.parse(2, std::vector<const char *>{"./app.exe", "10"}.data());
    REQUIRE(value == 10);
}

TEST_CASE("acore::CommandLine [examples]")
{
    SECTION("[usage]")
    {
        const int argc = 6;
        std::array<const char *, 6> args{"./app.exe", "-i", "main.cpp", "--flag", "--Include=/some/path", "--Include=\"other/path/with space/\""}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const char **argv = args.data();

        // clang-format off
//! [[Usage]]
//int main(int argc, char *argv[])
//argc: "./app.exe", "-i", "main.cpp", "--flag", "--Include=/some/path", "--Include=\"other/path/with space/\"
std::string input;
bool flag = false;
std::string output;
std::vector<std::string> includePaths;

acore::CommandLine commandLine;
commandLine.option().longName("input").shortName('i').required().description("The required input file").bindTo(&input);
commandLine.option().longName("flag").description("An optional flag.").bindTo(&flag);
commandLine.option().positional().defaultValue(std::string{"a.o"}).description("An optional defaulted output file.").bindTo(&output);
commandLine.option().longName("Include").description("Optional repeatable argument").bindTo(&includePaths);
commandLine.parse(argc, argv);

//intput == main.cpp
//flag == true
//output == a.o
//includePaths == {/some/path, other/path/with space/}
//! [[Usage]]
        // clang-format on
        REQUIRE(input == "main.cpp");
        REQUIRE(flag);
        REQUIRE(output == "a.o");
        REQUIRE(includePaths == std::vector<std::string>{"/some/path", "other/path/with space/"});
    }
}
}
