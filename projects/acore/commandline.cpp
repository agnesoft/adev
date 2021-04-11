#ifdef _MSC_VER
export module acore : commandline;

export import : commandline_option;
import : acore_common;
#endif

namespace acore
{
//! The `CommandLine` is the strongly typed command
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
//! - Long name is mandatory and must match the pattern `[a-zA-Z][a-zA-Z\\d]+` and always start with double dash, e.g. `--longName1`
//! - Short name is optional and must be a single alphabetic character `[a-zA-Z]` and always start with a single dash, e.g. `-l`
//! - Value can be provided using equal sign, e.g. `--longName1=value`
//! - Value can be provided as a next argument, e.g `-l value`
//! - Positional arguments are completely freeform
//! - Quoted values are unquoted
//! - Each option can match at most once unles it is bound to repeatable value, e.g. std::vector
//!
//! After all the desired options are defined call
//! parse() method with the application's main()
//! function arguments.
//!
//! Example:
//! \snippet commandlinetest.cpp [Usage]
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
export class CommandLine
{
public:
    //! After parsing returns the application's name
    //! otherwise an empty string. The name is extracted
    //! from the command() by stripping the path and
    //! the extension. E.g. ./app.exe -> app
    [[nodiscard]] auto applicationName() const noexcept -> const std::string &
    {
        return mAppName;
    }

    //! After parsing returns the first mandatory
    //! command line argument that represents the
    //! command starting the application unchanged.
    [[nodiscard]] auto command() const noexcept -> const std::string &
    {
        return mCommand;
    }

    //! Disables printing of the help in all cases.
    auto disableHelp() noexcept -> void
    {
        mHelpEnabled = false;
    }

    //! Enables printing of the help (this is the
    //! default).
    auto enableHelp() noexcept -> void
    {
        mHelpEnabled = true;
    }

    //! Returns true if the help has been printed
    //! after parsing.
    [[nodiscard]] auto helpDisplayed() const noexcept -> bool
    {
        return mHelpDisplayed;
    }

    //! Returns true if the help can be printed or
    //! false if it was disabled.
    [[nodiscard]] auto helpEnabled() const noexcept -> bool
    {
        return mHelpEnabled;
    }

    //! Creates a new option and returns the builder
    //! object for defining it.
    [[nodiscard]] auto option() -> CommandLineOption::Option
    {
        mOptions.emplace_back(CommandLineOption());
        return CommandLineOption::Option{&mOptions.back()};
    }

    //! Parses the command line using the same arguments
    //! as passed to main(). If `argc` is 0 an
    //! std::invalid_argument is thrown. If an error is
    //! encountered during parsing acore::runtime_error
    //! is thrown, it's text printed along with the
    //! availability of the help (unless disabled).
    //! Whenever the option is matched against an
    //! argument the argument is extracted and saved
    //! to the corresponding bound value.
    auto parse(int argc, const char *const *argv) -> void
    {
        try
        {
            parseCommand(argc, argv);
            parseArgs(argc, argv);
            parse();
        }
        catch (std::exception &e)
        {
            printParsingError(e);
            throw;
        }
    }

    //! Sets `stream` to be used for printing the
    //! errors and the help. By default std::cout
    //! is used. Use this method if you want to redirect
    //! the errors and/or help elsewhere.
    auto setPrintStream(std::ostream *stream) -> void
    {
        mStream = stream;
    }

private:
    class OptionHelpLine
    {
    public:
        OptionHelpLine() = default;
        explicit OptionHelpLine(const CommandLineOption *option) :
            mOption(option),
            mName(getName()),
            mAttributes(getAttributes())
        {
        }

        [[nodiscard]] auto option() const noexcept -> const CommandLineOption *
        {
            return mOption;
        }

        [[nodiscard]] auto name() const -> const std::string &
        {
            return mName;
        }

        [[nodiscard]] auto attributes() const -> const std::string &
        {
            return mAttributes;
        }

    private:
        [[nodiscard]] auto getName() const -> std::string
        {
            if (mOption->isPositional())
            {
                return "[positional]";
            }

            if (mOption->shortName() == char{})
            {
                return "--" + mOption->longName();
            }

            return std::string{"-"} + mOption->shortName() + ", --" + mOption->longName();
        }

        [[nodiscard]] auto getAttributes() const -> std::string
        {
            std::string attributes;

            if (mOption->isDefaulted())
            {
                return '[' + mOption->boundValueAsString() + '=' + mOption->defaultValueAsString() + ']';
            }

            return '[' + mOption->boundValueAsString() + ']';
        }

        const CommandLineOption *mOption = nullptr;
        std::string mName;
        std::string mAttributes;
    };

    struct Option
    {
        const CommandLineOption *option = nullptr;
        bool matched = false;
    };

    [[nodiscard]] static auto argument(const char *const *argv, int index) -> std::string
    {
        const char *const *arg = argv;
        std::advance(arg, index);
        return *arg;
    }

    [[nodiscard]] static auto countNameWidth(const std::vector<OptionHelpLine> &options) -> std::size_t
    {
        constexpr std::size_t minimumWidth = 10;
        std::size_t width = minimumWidth;

        for (const OptionHelpLine &line : options)
        {
            if (width < line.name().size())
            {
                width = line.name().size();
            }
        }

        return width;
    }

    [[nodiscard]] static auto countAttributesWidth(const std::vector<OptionHelpLine> &options) -> std::size_t
    {
        constexpr std::size_t minimumWidth = 10;
        std::size_t width = minimumWidth;

        for (const OptionHelpLine &line : options)
        {
            if (width < line.attributes().size())
            {
                width = line.attributes().size();
            }
        }

        return width;
    }

    [[nodiscard]] auto helpRequested() const -> bool
    {
        const std::vector<std::string> helpNames = {"-?"};
        return std::find_first_of(mArgs.cbegin(), mArgs.cend(), helpNames.cbegin(), helpNames.cend()) != mArgs.cend();
    }

    [[nodiscard]] auto matchArg(std::vector<std::string>::const_iterator *arg, std::vector<Option> *options) const -> bool
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

    [[nodiscard]] auto matchArgs() const -> std::vector<Option>
    {
        std::vector<Option> opts = options();

        for (auto arg = mArgs.cbegin(); arg != mArgs.cend();)
        {
            if (!matchArg(&arg, &opts))
            {
                throw std::runtime_error{std::string{"Unknown option '"} + *arg + '\''};
            }
        }

        return opts;
    }

    [[nodiscard]] auto optionHelpLines() -> std::vector<OptionHelpLine>
    {
        std::vector<OptionHelpLine> lines;

        for (const CommandLineOption &option : mOptions)
        {
            lines.emplace_back(OptionHelpLine{&option});
        }

        return lines;
    }

    [[nodiscard]] auto options() const -> std::vector<Option>
    {
        std::vector<Option> opts;

        for (const CommandLineOption &option : mOptions)
        {
            opts.emplace_back(Option{&option, false});
        }

        return opts;
    }

    auto parse() -> void
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

    auto parseArgs(int argc, const char *const *argv) -> void
    {
        mArgs.clear();

        for (int i = 1; i < argc; i++)
        {
            mArgs.emplace_back(argument(argv, i));
        }
    }

    auto parseCommand(int argc, const char *const *argv) -> void
    {
        if (argc == 0)
        {
            throw std::logic_error{"Missing mandatory first command line argument"};
        }

        mCommand = *argv;
        mAppName = std::filesystem::path{mCommand}.stem().string();
    }

    auto printHelp() -> void
    {
        printHelpHeader();
        printOptions(optionHelpLines());
        mHelpDisplayed = true;
    }

    auto printHelpHeader() -> void
    {
        constexpr int width = 15;

        printStream() << "Usage:\n    " << mAppName << " [options]" << '\n';
        printStream() << "Syntax:\n";
        printStream() << std::left << std::setw(width) << "  switch"
                      << "--switch, -s\n";
        printStream() << std::left << std::setw(width) << "  named"
                      << "--name=value, --n=value, --name value, --n value\n";
        printStream() << std::left << std::setw(width) << "  positional"
                      << "value\n";
        printStream() << "Options:" << '\n';
    }

    auto printHelpOption(std::size_t nameWidth, std::size_t attributesWidth) -> void
    {
        printStream() << "    " << std::left << std::setw(nameWidth) << "-?" << std::setw(attributesWidth) << "[switch]"
                      << "Prints this help.\n";
    }

    auto printOption(const OptionHelpLine &line, std::size_t nameWidth, std::size_t attributesWidth) -> void
    {
        printStream() << "    ";
        printStream() << std::left << std::setw(nameWidth) << line.name();
        printStream() << std::setw(attributesWidth) << line.attributes() << line.option()->description() << '\n';
    }

    auto printOptions(const std::vector<OptionHelpLine> &options) -> void
    {
        const std::size_t nameWidth = countNameWidth(options) + 5;
        const std::size_t attributesWidth = countAttributesWidth(options) + 5;
        printHelpOption(nameWidth, attributesWidth);
        printRequired(options, nameWidth, attributesWidth);
        printOptional(options, nameWidth, attributesWidth);
        printPositional(options, nameWidth, attributesWidth);
    }

    auto printOptional(const std::vector<OptionHelpLine> &options, std::size_t nameWidth, std::size_t attributesWidth) -> void
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

    auto printParsingError(const std::exception &e) -> void
    {
        printStream() << "ERROR: parsing command line arguments: " << e.what() << '\n';

        if (mHelpEnabled)
        {
            printStream() << "Use -? to list the command line options.\n";
        }
    }

    auto printPositional(const std::vector<OptionHelpLine> &options, std::size_t nameWidth, std::size_t attributesWidth) -> void
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

    auto printRequired(const std::vector<OptionHelpLine> &options, std::size_t nameWidth, std::size_t attributesWidth) -> void
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

    [[nodiscard]] auto printStream() -> std::ostream &
    {
        if (mStream != nullptr)
        {
            return *mStream;
        }

        return std::cout;
    }

    static auto verifyOptions(const std::vector<Option> &options) -> void
    {
        for (Option opt : options)
        {
            if (!opt.matched)
            {
                if (opt.option->isRequired())
                {
                    throw std::runtime_error{std::string{"Option '"} + opt.option->name() + "' was set as required but did not match any arguments"};
                }

                opt.option->defaultBoundValue();
            }
        }
    }

    std::string mCommand;
    std::string mAppName;
    std::vector<std::string> mArgs;
    std::vector<CommandLineOption> mOptions;
    std::ostream *mStream = nullptr;
    bool mHelpEnabled = true;
    bool mHelpDisplayed = false;
};
}
