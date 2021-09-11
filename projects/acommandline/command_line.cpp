#ifndef __clang__
export module acommandline : command_line;
import : option_builder;
import : option_matcher;
import : option_setter;
import : printer;
#endif

namespace acommandline
{
export class CommandLine
{
public:
    CommandLine() :
        CommandLine(std::cout)
    {
    }

    explicit CommandLine(std::ostream &stream) :
        printer{stream}
    {
    }

    [[nodiscard]] auto application_name() const noexcept -> const std::string &
    {
        return this->appName;
    }

    [[nodiscard]] auto command() const noexcept -> const std::string &
    {
        return this->cmd;
    }

    [[nodiscard]] auto option() -> OptionBuilder
    {
        return OptionBuilder{this->options.emplace_back(Option{})};
    }

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
        for (Option &option : this->options)
        {
            if (!option.matched || ::acommandline::is_repeated(option))
            {
                if (OptionMatcher::match(option, argument) && OptionSetter::set_value(option, argument, this->arguments.cend()))
                {
                    option.matched = true;
                }
            }
        }

        throw std::runtime_error{std::string{"Unknown option '"} + **argument + "'."};
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
