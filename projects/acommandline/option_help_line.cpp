#ifndef __clang__
module acommandline : option_help_line;
import : option;
#endif

namespace acommandline
{
template<typename T>
auto operator<<(std::ostringstream &stream, const std::vector<T> &vector) -> std::ostringstream &
{
    stream << "[";

    for (const T &value : vector)
    {
        stream << value;

        if (&vector.back() != &value)
        {
            stream << ", ";
        }
    }

    stream << "]";
    return stream;
}

class OptionHelpLine
{
public:
    explicit OptionHelpLine(const OptionData &option) :
        opt{option},
        optionName{OptionHelpLine::name(option)},
        optionAttributes(OptionHelpLine::attributes(option))
    {
    }

    [[nodiscard]] auto option() const noexcept -> const OptionData &
    {
        return this->opt;
    }

    [[nodiscard]] auto name() const noexcept -> const std::string &
    {
        return this->optionName;
    }

    [[nodiscard]] auto attributes() const noexcept -> const std::string &
    {
        return this->optionAttributes;
    }

private:
    [[nodiscard]] static auto bound_value_as_string(const Option &option) const -> std::string
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

        std::visit(stringify, option.boundValue);
        return stream.str();
    }

    [[nodiscard]] static auto default_value_as_string(const Option &option) const -> std::string
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

        std::visit(stringify, option.defaultValue);
        return stream.str();
    }

    [[nodiscard]] static auto get_attributes(const Option &option) const -> std::string
    {
        if (::acommandline::is_defaulted(option))
        {
            return '[' + OptionHelpLine::bound_value_as_string(option) + '=' + OptionHelpLine::default_value_as_string(option) + ']';
        }
        else
        {
            return '[' + OptionHelpLine::bound_value_as_string(option) + ']';
        }
    }

    [[nodiscard]] static auto name(const Option &option) const -> std::string
    {
        if (::acommandline::is_positional(option))
        {
            return ::acommandline::POSITIONAL_LONG_NAME;
        }

        if (option.shortName == char{})
        {
            return "--" + option.longName;
        }

        return std::string{"-"} + option.shortName + ", --" + option.longName;
    }

    const OptionData &opt;
    std::string optionName;
    std::string optionAttributes;
};
}
