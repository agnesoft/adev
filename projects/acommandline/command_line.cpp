#ifndef __clang__
export module acommandline : command_line;
import : option_builder;
import : option_matcher;
import : option_setter;
import : printer;
#endif

namespace acommandline
{
//! The CommandLine class is a strongly typed
//! command line parser with builder interface for
//! defining command line options.
//!
//! The CommandLine can be default constructed in
//! which case it will use `std::cout` as its
//! output stream. Alternatively an `std::ostream
//! &` can be supplied to the constructor.
//!
//! # Options
//!
//! The options are defined by calling option()
//! method on the CommandLine instance. It will
//! create a new option and it will return a
//! builder object that will guide through the
//! option definition. At the end the option must
//! be bound to an existing variable of one of
//! these types:
//!
//! - std::string
//! - std::int64_t
//! - double
//! - std::vector<std::string>
//! - std::vector<std::int64_t>
//! - std::vector<double>
//!
//! When any `std::vector` value is used the
//! option is considered `repeatable` and can be
//! matched multiple times during parsing. You can
//! also specify a default value for an option
//! making it optional or set it as required.
//!
//! The rules for the command line options:
//!
//! - Long name is mandatory for named options and
//!   must match the pattern
//!   `[a-zA-Z][a-zA-Z\\d]+`. Long names are
//!   expected to be prefixed with `--`, e.g.
//!   `--longName1`
//! - Short name is optional and must be a single
//!   alphabetic character `[a-zA-Z]`. Short names
//!   are expected to be prefixed with `-`, e.g.
//!   `-l`
//! - Option without a name is considered a
//!   positional and can be matched at any
//!   position except for the first argument (the
//!   command) and the values of named options
//!   (see below).
//! - Value to a named option can be provided
//!   using the equal sign, e.g.
//!   `--longName1=value`
//! - Value to a named options can be provided as
//!   a next argument, e.g `-l value`
//! - Quoted values are unquoted
//! - Each option can match at most once unless it
//!   is bound to repeatable value
//! - An option can be given a default value. Such
//!   an option cannot be required.
//! - An option can be made required. Such an
//!   option cannot have a default value.
//!
//! # Parsing
//!
//! After all the desired options are defined call
//! parse() method with the application's main()
//! function arguments.
//!
//! All options are optional unless defined
//! explicitly as required. When an option is not
//! matched and it has a default value specified
//! this value will be used. If no default value
//! is specified no value will be saved to the
//! bound variable. Repeatable options can be
//! matched multiple times and the values are
//! appended to the bound variable (an
//! `std::vector`).
//!
//! An exception will be thrown on any error
//! encountered during parsing. Common errors are:
//!
//! - Argument did not match any of the defined
//!   options (this also happens when multiple
//!   arguments would match single non-repeatable
//!   option)
//! - Argument value cannot be saved or converted
//!   to the bound variable
//! - Required option not matched
//!
//! # Help
//!
//! The CommandLine class generates help
//! automatically from the defined options and
//! their properties. The help is displayed using
//! hardcoded `-?` command line argument. When
//! this argument is used only the help is
//! displayed and no parsing is done.
//!
//! # Examples
//!
//!
export class CommandLine
{
public:
    //! Constructs the CommandLine with
    //! `std::cout` stream to print output.
    CommandLine() :
        CommandLine(std::cout)
    {
    }

    //! Constructs the CommandLine with custom
    //! `stream` to print output.
    explicit CommandLine(std::ostream &stream) :
        printer{stream}
    {
    }

    //! Returns the the first command line
    //! argument without the path component and
    //! extension.
    //!
    //! Examples:
    //!
    //! - `./my_app` -> `my_app`
    //! - `C:/my_app.exe` -> `my_app`
    [[nodiscard]] auto application_name() const noexcept -> const std::string &
    {
        return this->appName;
    }

    //! Returns the first command line argument.
    [[nodiscard]] auto command() const noexcept -> const std::string &
    {
        return this->cmd;
    }

    //! Starts composition of a new option for
    //! this command line. Returns the builder
    //! object. See CommandLine class description
    //! for the option building.
    [[nodiscard]] auto option() & -> OptionBuilder
    {
        return OptionBuilder{this->options.emplace_back(Option{})};
    }

    //! Parses the command line arguments that
    //! `main()` accepts (`argc` and `argv`). It
    //! matches the arguments to options defined
    //! by calls to option(). The values are
    //! stored in the values bound to the defined
    //! options. If an argument could not be
    //! matched to any of the option or if
    //! matching fails an exception is thrown. See
    //! CommandLine class description for the
    //! matching rules.
    auto parse(int argc, const char *const *argv) -> void
    {
        try
        {
            this->parse_command(argc, argv);
            this->parse_arguments(argc, argv);
            this->parse();
        }
        catch (const std::exception &error)
        {
            this->print_parsing_error(error);
            throw;
        }
    }

private:
    auto clear_matches() -> void
    {
        for (Option &option : this->options)
        {
            option.matched = false;
        }
    }

    [[nodiscard]] auto help_requested() const -> bool
    {
        const std::vector<std::string> helpNames = {"-?"};
        return std::find_first_of(this->arguments.cbegin(),
                                  this->arguments.cend(),
                                  helpNames.cbegin(),
                                  helpNames.cend())
            != this->arguments.cend();
    }
    auto match_argument(std::vector<std::string>::const_iterator *argument) -> void
    {
        const std::string argumentValue = **argument;

        for (Option &option : this->options)
        {
            if (!option.matched || ::acommandline::is_repeated(option))
            {
                if (OptionMatcher::match(option, argumentValue) && OptionSetter::set_value(option, argument, this->arguments.cend()))
                {
                    option.matched = true;
                }
            }
        }

        throw std::runtime_error{std::string{"Unknown option '"} + argumentValue + "'."};
    }

    auto match_arguments() -> void
    {
        for (auto argument = this->arguments.cbegin(); argument != this->arguments.cend();)
        {
            this->match_argument(&argument);
        }
    }

    auto parse() -> void
    {
        if (this->help_requested())
        {
            this->printer.print_help(this->appName, this->options);
        }
        else
        {
            this->clear_matches();
            this->match_arguments();
            this->validate_options();
        }
    }

    [[nodiscard]] static auto parse_argument(const char *const *argv, int index) -> std::string
    {
        const char *const *arg = argv;
        std::advance(arg, index);
        return *arg;
    }

    auto parse_arguments(int argc, const char *const *argv) -> void
    {
        this->arguments.clear();

        for (int i = 1; i < argc; i++)
        {
            this->arguments.emplace_back(CommandLine::parse_argument(argv, i));
        }
    }

    auto parse_command(int argc, const char *const *argv) -> void
    {
        if (argc == 0)
        {
            throw std::logic_error{"Missing mandatory first command line argument."};
        }

        this->cmd = *argv;
        this->appName = std::filesystem::path{this->cmd}.stem().string();
    }

    auto print_parsing_error(const std::exception &error) -> void
    {
        this->printer.print_parsing_error(error);
        this->printer.print_help_hint();
    }

    auto validate_options() -> void
    {
        for (Option &option : this->options)
        {
            if (!option.matched)
            {
                if (option.required)
                {
                    throw std::runtime_error{std::string{"Option '"} + option.longName + "' was set as required but did not match any arguments."};
                }

                OptionSetter::default_bound_value(option);
            }
        }
    }

    std::string cmd;
    std::string appName;
    std::vector<std::string> arguments;
    std::vector<Option> options;
    Printer printer;
};
}
