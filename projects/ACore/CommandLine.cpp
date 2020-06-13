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

#include "CommandLine.hpp"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace acore
{
auto CommandLine::applicationName() const noexcept -> const std::string &
{
    return mAppName;
}

auto CommandLine::disableHelp() noexcept -> void
{
    mHelpEnabled = false;
}

auto CommandLine::enableHelp() noexcept -> void
{
    mHelpEnabled = true;
}

auto CommandLine::command() const noexcept -> const std::string &
{
    return mCommand;
}

auto CommandLine::helpDisplayed() const noexcept -> bool
{
    return mHelpDisplayed;
}

auto CommandLine::helpEnabled() const noexcept -> bool
{
    return mHelpEnabled;
}

CommandLineOption::Option CommandLine::option()
{
    mOptions.emplace_back(CommandLineOption());
    return CommandLineOption::Option{&mOptions.back()};
}

auto CommandLine::parse(int argc, const char *const *argv) -> void
{
    try
    {
        parseCommand(argc, argv);
        parseArgs(argc, argv);
        parse();
    }
    catch (Exception &e)
    {
        printParsingError(e);
        throw e;
    }
}

auto CommandLine::setPrintStream(std::ostream *stream) -> void
{
    mStream = stream;
}

auto CommandLine::countNameWidth(const std::vector<OptionHelpLine> &options) -> std::size_t
{
    std::size_t width = 10;

    for (const OptionHelpLine &line : options)
    {
        if (width < line.name().size())
        {
            width = line.name().size();
        }
    }

    return width;
}

auto CommandLine::countAttributesWidth(const std::vector<OptionHelpLine> &options) -> std::size_t
{
    std::size_t width = 10;

    for (const OptionHelpLine &line : options)
    {
        if (width < line.attributes().size())
        {
            width = line.attributes().size();
        }
    }

    return width;
}

auto CommandLine::helpRequested() const -> bool
{
    const std::vector<std::string> helpNames = {"-?"};
    return std::find_first_of(mArgs.cbegin(), mArgs.cend(), helpNames.cbegin(), helpNames.cend()) != mArgs.cend();
}

auto CommandLine::matchArg(std::vector<std::string>::const_iterator *arg, std::vector<Option> *options) const -> bool
{
    for (auto opt = options->begin(); opt != options->end(); ++opt)
    {
        opt->matched = opt->option->match(arg, mArgs.cend());

        if (opt->matched)
        {
            if (!opt->option->isRepeated())
            {
                opt = options->erase(opt);
            }

            return true;
        }
    }

    return false;
}

auto CommandLine::matchArgs() const -> std::vector<Option>
{
    std::vector<Option> opts = options();

    for (auto arg = mArgs.cbegin(); arg != mArgs.cend();)
    {
        if (!matchArg(&arg, &opts))
        {
            throw Exception{} << "Unknown option '" << *arg << '\'';
        }
    }

    return opts;
}

auto CommandLine::optionHelpLines() -> std::vector<CommandLine::OptionHelpLine>
{
    std::vector<OptionHelpLine> lines;

    for (const CommandLineOption &option : mOptions)
    {
        lines.emplace_back(OptionHelpLine{&option});
    }

    return lines;
}

auto CommandLine::parse() -> void
{
    if (helpRequested())
    {
        printHelp();
    }
    else
    {
        verifyOptions(matchArgs());
    }
}

auto CommandLine::parseArgs(int argc, const char *const *argv) -> void
{
    mArgs.clear();

    for (int i = 1; i < argc; i++)
    {
        mArgs.emplace_back(argv[i]);
    }
}

auto CommandLine::parseCommand(int argc, const char *const *argv) -> void
{
    if (argc == 0)
    {
        throw Exception{} << "Missing mandatory first command line argument";
    }

    mCommand = argv[0];
    mAppName = std::filesystem::path{mCommand}.stem().string();
}

auto CommandLine::options() const -> std::vector<Option>
{
    std::vector<Option> opts;

    for (const CommandLineOption &option : mOptions)
    {
        opts.emplace_back(Option{&option, false});
    }

    return opts;
}

auto CommandLine::printHelp() -> void
{
    printHelpHeader();
    printOptions(optionHelpLines());
    mHelpDisplayed = true;
}

auto CommandLine::printHelpHeader() -> void
{
    printStream() << "Usage:\n    " << mAppName << " [options]" << '\n';
    printStream() << "Syntax:\n";
    printStream() << std::left << std::setw(15) << "  switch"
                  << "--switch, -s\n";
    printStream() << std::left << std::setw(15) << "  named"
                  << "--name=value, --n=value, --name value, --n value\n";
    printStream() << std::left << std::setw(15) << "  positional"
                  << "value\n";
    printStream() << "Options:" << '\n';
}

auto CommandLine::printHelpOption(std::size_t nameWidth, std::size_t attributesWidth) -> void
{
    printStream() << "    " << std::left << std::setw(nameWidth) << "-?" << std::setw(attributesWidth) << "[switch]"
                  << "Prints this help.\n";
}

auto CommandLine::printOption(const OptionHelpLine &line, std::size_t nameWidth, std::size_t attributesWidth) -> void
{
    printStream() << "    ";
    printStream() << std::left << std::setw(nameWidth) << line.name();
    printStream() << std::setw(attributesWidth) << line.attributes() << line.option()->description() << '\n';
}

auto CommandLine::printOptions(const std::vector<OptionHelpLine> &options) -> void
{
    const std::size_t nameWidth = countNameWidth(options) + 5;
    const std::size_t attributesWidth = countAttributesWidth(options) + 5;
    printHelpOption(nameWidth, attributesWidth);
    printRequired(options, nameWidth, attributesWidth);
    printOptional(options, nameWidth, attributesWidth);
    printPositional(options, nameWidth, attributesWidth);
}

auto CommandLine::printRequired(const std::vector<OptionHelpLine> &options, std::size_t nameWidth, std::size_t attributesWidth) -> void
{
    printStream() << "  Required:\n";

    for (const OptionHelpLine &line : options)
    {
        if (line.option()->isRequired())
        {
            printOption(line, nameWidth, attributesWidth);
        }
    }
}

auto CommandLine::printOptional(const std::vector<OptionHelpLine> &options, std::size_t nameWidth, std::size_t attributesWidth) -> void
{
    printStream() << "  Optional:\n";

    for (const OptionHelpLine &line : options)
    {
        if (!line.option()->isRequired() && !line.option()->isPositional())
        {
            printOption(line, nameWidth, attributesWidth);
        }
    }
}

auto CommandLine::printPositional(const std::vector<OptionHelpLine> &options, std::size_t nameWidth, std::size_t attributesWidth) -> void
{
    printStream() << "  Positional:\n";

    for (const OptionHelpLine &line : options)
    {
        if (line.option()->isPositional())
        {
            printOption(line, nameWidth, attributesWidth);
        }
    }
}

auto CommandLine::printParsingError(const Exception &e) -> void
{
    printStream() << "ERROR parsing command line arguments: " << e.what() << '\n';

    if (mHelpEnabled)
    {
        printStream() << "Use -? to list the command line options.\n";
    }
}

auto CommandLine::printStream() -> std::ostream &
{
    if (mStream)
    {
        return *mStream;
    }

    return std::cout;
}

auto CommandLine::verifyOptions(const std::vector<Option> &options) const -> void
{
    for (Option opt : options)
    {
        if (!opt.matched)
        {
            if (opt.option->isRequired())
            {
                throw Exception{} << "Option '" << opt.option->name() << "' was set as required but did not match any arguments";
            }
            else
            {
                opt.option->defaultBoundValue();
            }
        }
    }
}

CommandLine::OptionHelpLine::OptionHelpLine(const CommandLineOption *option) :
    mOption(option),
    mName(getName()),
    mAttributes(getAttributes())
{
}

auto CommandLine::OptionHelpLine::getName() const -> std::string
{
    if (mOption->isPositional())
    {
        return "[positional]";
    }
    else if (mOption->shortName() == char{})
    {
        return "--" + mOption->longName();
    }
    else
    {
        return std::string{"-"} + mOption->shortName() + ", --" + mOption->longName();
    }
}

auto CommandLine::OptionHelpLine::getAttributes() const -> std::string
{
    std::string attributes;

    if (mOption->isDefaulted())
    {
        return '[' + mOption->boundValueAsString() + '=' + mOption->defaultValueAsString() + ']';
    }

    return '[' + mOption->boundValueAsString() + ']';
}

auto CommandLine::OptionHelpLine::option() const noexcept -> const CommandLineOption *
{
    return mOption;
}

auto CommandLine::OptionHelpLine::name() const -> const std::string &
{
    return mName;
}

auto CommandLine::OptionHelpLine::attributes() const -> const std::string &
{
    return mAttributes;
}
}
