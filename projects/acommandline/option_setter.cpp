#ifndef __clang__
module acommandline : option_setter;
import : option;
#endif

namespace acommandline
{
//! \private
class OptionSetter
{
public:
    static auto default_bound_value(Option &option) -> void
    {
        const auto valueSetter = [&](auto &&boundVal) {
            using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundVal)>>;

            if constexpr (!std::is_same_v<BoundT, std::monostate>)
            {
                OptionSetter::set_bound_value_to_default<BoundT>(option, boundVal);
            }
            else
            {
                throw std::runtime_error{std::string{"The option "} + option.longName + " is missing a bound value."};
            }
        };

        std::visit(valueSetter, option.boundValue);
    }

    [[nodiscard]] static auto set_value(Option &option, std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) -> bool
    {
        if (OptionSetter::set_value(option, OptionSetter::extract_value(option, argument, end)))
        {
            ++(*argument);
            return true;
        }

        return false;
    }

private:
    static auto advance_argument_iterator(const Option &option, std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) -> void
    {
        if (++(*argument) == end)
        {
            throw std::runtime_error{std::string{"Missing value for option '"} + option.longName + "'."};
        }
    }

    [[nodiscard]] static auto extract_named_value(const std::string &argument) -> std::string
    {
        const size_t pos = argument.find('=');

        if (pos != std::string::npos)
        {
            return argument.substr(pos + 1);
        }

        return {};
    }

    [[nodiscard]] static auto extract_value(const Option &option, std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) -> std::string
    {
        std::string value = OptionSetter::extract_value(option, **argument);

        if (!OptionSetter::has_value(option, value))
        {
            OptionSetter::advance_argument_iterator(option, argument, end);
            return **argument;
        }

        return value;
    }

    [[nodiscard]] static auto extract_value(const Option &option, const std::string &argument) -> std::string
    {
        if (::acommandline::is_positional(option))
        {
            return argument;
        }

        return OptionSetter::extract_named_value(argument);
    }

    static auto handle_set_option_failure(const Option &option, [[maybe_unused]] const std::exception &error, const std::string &value) -> void
    {
        if (!::acommandline::is_positional(option))
        {
            const auto throwError = [&](auto &&boundValue) {
                using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;
                throw std::runtime_error{std::string{"Failed to set option '"} + option.longName + "' (" + typeid(BoundT).name() + ") from value '" + value + "'."};
            };

            std::visit(throwError, option.boundValue);
        }
    }

    [[nodiscard]] static auto has_value(const Option &option, const std::string &value) -> bool
    {
        return !value.empty() || ::acommandline::is_switch(option);
    }

    template<typename T>
    static auto set_bound_value_to_default(Option &option, T *boundVal) -> void
    {
        const auto valueSetter = [&](auto &&defaultValue) {
            using DefaultT = std::decay_t<decltype(defaultValue)>;

            if constexpr (std::is_same_v<T, DefaultT>)
            {
                *boundVal = defaultValue;
            }
        };

        std::visit(valueSetter, option.defaultValue);
    }

    [[nodiscard]] static auto set_value(Option &option, const std::string &value) -> bool
    {
        const auto valueSetter = [&](auto &&boundValue) {
            using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;

            if constexpr (std::is_same_v<BoundT, std::monostate>)
            {
                throw std::runtime_error{std::string{"Bind value undefined for option '"} + option.longName + "'."};
            }
            else if constexpr (std::is_same_v<BoundT, bool>)
            {
                *boundValue = true;
            }
            else if constexpr (std::is_same_v<BoundT, double>)
            {
                *boundValue = std::stod(value);
            }
            else if constexpr (std::is_same_v<BoundT, std::int64_t>)
            {
                *boundValue = std::stoll(value);
            }
            else if constexpr (std::is_same_v<BoundT, std::string>)
            {
                *boundValue = OptionSetter::unquote(value);
            }
            else if constexpr (std::is_same_v<BoundT, std::vector<std::int64_t>>)
            {
                (*boundValue).emplace_back(std::stoll(value));
            }
            else if constexpr (std::is_same_v<BoundT, std::vector<double>>)
            {
                (*boundValue).emplace_back(std::stod(value));
            }
            else if constexpr (std::is_same_v<BoundT, std::vector<std::string>>)
            {
                (*boundValue).emplace_back(OptionSetter::unquote(value));
            }
        };

        try
        {
            std::visit(valueSetter, option.boundValue);
            return true;
        }
        catch ([[maybe_unused]] const std::runtime_error &error)
        {
            throw;
        }
        catch (const std::exception &error)
        {
            OptionSetter::handle_set_option_failure(option, error, value);
        }

        return false;
    }

    [[nodiscard]] static auto is_quoted(const std::string &value) noexcept -> bool
    {
        return value.size() >= 2 && value.front() == '"' && value.back() == '"';
    }

    [[nodiscard]] static auto unquote(const std::string &value) -> std::string
    {
        if (OptionSetter::is_quoted(value))
        {
            return std::string{value.begin() + 1, value.end() - 1};
        }

        return value;
    }
};
}