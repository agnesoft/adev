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

#ifndef ACORE_COMMANDLINE_HPP
#define ACORE_COMMANDLINE_HPP

#include "CommandLineOption.hpp"
#include "Exception.hpp"

namespace acore
{
//! The \c CommandLine is the strongly typed command
//! line parser with fluent interface.
//!
//! First you need to instantiate the CommandLine
//! and then define the options using the option()
//! method. It will return a builder object that will
//! guide you through defining each option using the
//! fluent interface. It ensures that each option
//! is correctly defined. The options can be either
//! named or positional. Each defined option needs
//! to be bound to a value of specific type. The
//! supported types are:
//!
//! - std::string
//! - std::int64_t
//! - double
//! - std::vector<std::string>
//! - std::vector<std::int64_t>
//! - std::vector<double>
//!
//! When a vector variant is bound the option is
//! considered "repeatable" and can be matched more
//! than once during parsing. You can always specify
//! required options or default values for options
//! that are unmatched (and therefore optional). The
//! CommandLine also generated help from the options
//! that documents the usage, syntax, types, names,
//! required arguments, default values and descriptions
//! automatically. It is hardcoded to use -? syntax.
//! See CommandLineOption for further details on the
//! parameters and restrictions.
//!
//! The CommandLine's supported syntax is intentionally
//! somewhat strict in how the arguments must be provided.
//! The  overview of the supported syntax:
//!
//! - Long name is mandatory and must match the pattern <tt> [a-zA-Z][a-zA-Z\\d]+ </tt> and always start with double dash, <tt> e.g. --longName1 </tt>
//! - Short name is optional and must be a single alphabetic character <tt> [a-zA-Z] </tt> and always start with a single dash, <tt> e.g. -l </tt>
//! - Value can be provided using equal sign, <tt> e.g. --longName1=value </tt>
//! - Value can be provided as a next argument, <tt> e.g -l value </tt>
//! - Positional arguments are completely freeform
//! - Quoted values are unquoted
//! - Each option can match at most once unles it is bound to repeatable value, e.g. acore::Vector
//!
//! After all the desired options are defined call
//! parse() method with the application's main()
//! function arguments.
//!
//! Example:
//! \snippet CommandLineTest.cpp [Usage]
//!
//! The parsing may fail for many reasons that are
//! all handled using exceptions:
//!
//! - Required argument was not provided
//! - Value was not provided for a named option
//! - Type mismatch of provided value and the argument (e.g non-integer string to an integer value)
//! - Unmatched argument
//! - Syntax error (usually interpreted as missing value or value of incorrect type)
//!
//! In all of these cases the CommandLine will output
//! the text of the exception along with the suggestion
//! to use -? to see details about the options.
class CommandLine
{
public:
    //! After parsing returns the application's name
    //! otherwise an empty string. The name is extracted
    //! from the command() by stripping the path and
    //! the extension. E.g. ./app.exe -> app
    [[nodiscard]] auto applicationName() const noexcept -> const std::string &;

    //! After parsing returns the first mandatory
    //! command line argument that represents the
    //! command starting the application unchanged.
    [[nodiscard]] auto command() const noexcept -> const std::string &;

    //! Disables printing of the help in all cases.
    auto disableHelp() noexcept -> void;

    //! Enables printing of the help (this is the
    //! default).
    auto enableHelp() noexcept -> void;

    //! Returns true if the help has been printed
    //! after parsing.
    [[nodiscard]] auto helpDisplayed() const noexcept -> bool;

    //! Returns true if the help can be printed or
    //! false if it was disabled.
    [[nodiscard]] auto helpEnabled() const noexcept -> bool;

    //! Creates a new option and returns the builder
    //! object for defining it.
    [[nodiscard]] auto option() -> CommandLineOption::Option;

    //! Parses the command line using the same arguments
    //! as passed to main(). If \a argc is 0 an
    //! acore::Exception is thrown. If an error is
    //! encountered during parsing acore::Exception
    //! is thrown, it's text printed along with the
    //! availability of the help (unless disabled).
    //! Whenever the option is matched against an
    //! argument the argument is extracted and saved
    //! to the corresponding bound value.
    auto parse(int argc, const char *const *argv) -> void;

    //! Sets \a stream to be used for printing the
    //! errors and the help. By default std::cout
    //! is used. Use this method if you want to redirect
    //! the errors and/or help elsewhere.
    auto setPrintStream(std::ostream *stream) -> void;

private:
    class OptionHelpLine
    {
    public:
        OptionHelpLine() = default;
        explicit OptionHelpLine(const CommandLineOption *option);

        [[nodiscard]] auto option() const noexcept -> const CommandLineOption *;
        [[nodiscard]] auto name() const -> const std::string &;
        [[nodiscard]] auto attributes() const -> const std::string &;

    private:
        [[nodiscard]] auto getName() const -> std::string;
        [[nodiscard]] auto getAttributes() const -> std::string;

        const CommandLineOption *mOption = nullptr;
        std::string mName;
        std::string mAttributes;
    };

    struct Option
    {
        const CommandLineOption *option = nullptr;
        bool matched = false;
    };

    [[nodiscard]] static auto argument(const char *const *argv, int index) -> std::string;
    [[nodiscard]] static auto countNameWidth(const std::vector<OptionHelpLine> &options) -> std::size_t;
    [[nodiscard]] static auto countAttributesWidth(const std::vector<OptionHelpLine> &options) -> std::size_t;
    [[nodiscard]] auto helpRequested() const -> bool;
    [[nodiscard]] auto matchArg(std::vector<std::string>::const_iterator *arg, std::vector<Option> *options) const -> bool;
    [[nodiscard]] auto matchArgs() const -> std::vector<Option>;
    [[nodiscard]] auto optionHelpLines() -> std::vector<OptionHelpLine>;
    [[nodiscard]] auto options() const -> std::vector<Option>;
    auto parse() -> void;
    auto parseArgs(int argc, const char *const *argv) -> void;
    auto parseCommand(int argc, const char *const *argv) -> void;
    auto printHelp() -> void;
    auto printHelpHeader() -> void;
    auto printHelpOption(std::size_t nameWidth, std::size_t attributesWidth) -> void;
    auto printOption(const OptionHelpLine &line, std::size_t nameWidth, std::size_t attributesWidth) -> void;
    auto printOptions(const std::vector<OptionHelpLine> &options) -> void;
    auto printOptional(const std::vector<OptionHelpLine> &options, std::size_t nameWidth, std::size_t attributesWidth) -> void;
    auto printParsingError(const Exception &e) -> void;
    auto printPositional(const std::vector<OptionHelpLine> &options, std::size_t nameWidth, std::size_t attributesWidth) -> void;
    auto printRequired(const std::vector<OptionHelpLine> &options, std::size_t nameWidth, std::size_t attributesWidth) -> void;
    [[nodiscard]] auto printStream() -> std::ostream &;
    static auto verifyOptions(const std::vector<Option> &options) -> void;

    std::string mCommand;
    std::string mAppName;
    std::vector<std::string> mArgs;
    std::vector<CommandLineOption> mOptions;
    std::ostream *mStream = nullptr;
    bool mHelpEnabled = true;
    bool mHelpDisplayed = false;
};
}

#endif
