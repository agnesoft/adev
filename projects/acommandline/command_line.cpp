#ifndef __clang__
export module acommandline : command_line;
import : option_builder;
import : option_matcher;
import : option_setter;
#endif

namespace acommandline
{
export class CommandLine
{
public:
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
        return OptionBuilder{this->options.emplace_back(OptionData{})};
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
        for (OptionData &option : this->options)
        {
            option.matched = false;
        }
    }

    [[nodiscard]] static constexpr auto is_repeated(const OptionData &option) -> bool
    {
        return std::holds_alternative<std::vector<std::int64_t> *>(this->bound_value())
            || std::holds_alternative<std::vector<double> *>(this->bound_value())
            || std::holds_alternative<std::vector<std::string> *>(this->bound_value());
    }

    [[nodiscard]] auto match_argument(std::vector<std::string>::const_iterator *arg) const -> void
    {
        for (OptionData &option : this->options)
        {
            if (!option.matched || CommandLine::is_repeated(option))
            {
                if (option.matched = OptionMatcher{option}.match(arg))
                {
                    OptionSetter{option}.set_value(arg, this->arguments.cend());
                    return true;
                }
            }
        }

        throw std::runtime_error{std::string{"Unknown option '"} + *arg + "'."};
    }

    [[nodiscard]] auto match_arguments() const -> void
    {
        for (auto argument = this->arguments.cbegin(); arg != this->arguments.cend();)
        {
            this->match_argument(&argument);
        }
    }

    auto parse() -> void
    {
        // if (this->help_requested())
        // {
        //     this->print_help();
        // }
        // else
        // {
        this->clear_matches();
        this->match_arguments();
        this->validate_options();
        // }
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

    auto print_parsing_error(const std::exception &e) -> void
    {
        //TODO
    }

    auto validate_options() -> void
    {
        for (const OptionData &option : this->options)
        {
            if (!option.matched)
            {
                if (option.required)
                {
                    throw std::runtime_error{std::string{"Option '"} + option.longName + "' was set as required but did not match any arguments."};
                }

                OptionSetter{option}.default_bound_value();
            }
        }
    }

    std::string cmd;
    std::string appName;
    std::vector<std::string> arguments;
    std::vector<OptionData> options;
};
}
