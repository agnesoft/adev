#ifndef __clang__
module acommandline : option_help_line;
import : option_data;
#endif

namespace acommandline
{
class OptionHelpLine
{
public:
    explicit OptionHelpLine(const OptionData &option) :
        opt{option},
        optionName{this->get_name()},
        optionAttributes(this->get_attributes())
    {
    }

    [[nodiscard]] auto option() const noexcept -> const OptionData *
    {
        return this->opt;
    }

    [[nodiscard]] auto name() const -> const std::string &
    {
        return this->optionName;
    }

    [[nodiscard]] auto attributes() const -> const std::string &
    {
        return this->optionAttributes;
    }

private:
    [[nodiscard]] auto bound_value_as_string() const -> std::string
    {
        std::stringstream stream;
        const auto stringify = [&](auto &&value) {
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
        };

        std::visit(stringify, this->opt.boundValue);
        return stream.str();
    }

    [[nodiscard]] auto default_value_as_string() const -> std::string
    {
        std::ostringstream stream;

        const auto stringify = [&](auto &&value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, bool>)
            {
                stream << (value ? "true" : "false");
            }
            else if constexpr (!std::is_same_v<T, std::monostate>)
            {
                stream << value;
            }
        };

        std::visit(stringify, this->opt.defaultValue);
        return stream.str();
    }

    [[nodiscard]] auto get_name() const -> std::string
    {
        if (this->is_positional())
        {
            return "[positional]";
        }

        if (this->opt.shortName == char{})
        {
            return "--" + this->opt.longName;
        }

        return std::string{"-"} + this->opt.shortName + ", --" + this->opt.longName;
    }

    [[nodiscard]] auto get_attributes() const -> std::string
    {
        if (this->is_defaulted())
        {
            return '[' + this->bound_value_as_string() + '=' + this->default_value_as_string() + ']';
        }
        else
        {
            return '[' + this->bound_value_as_string() + ']';
        }
    }

    [[nodiscard]] auto is_defaulted() const noexcept -> bool
    {
        return !std::holds_alternative<std::monostate>(this->opt.defaultValue);
    }

    [[nodiscard]] auto is_positional() const noexcept -> bool
    {
        return this->opt.longName == "[positional]";
    }

    const OptionData &opt;
    std::string optionName;
    std::string optionAttributes;
};
}
