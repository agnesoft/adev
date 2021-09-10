#ifndef __clang__
module acommandline : option_setter;
import : option_base;
#endif

namespace acommandline
{
class OptionSetter : private OptionBase
{
public:
    auto default_bound_value() const -> void
    {
        const auto valueSetter = [&](auto &&boundVal) {
            using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundVal)>>;

            if constexpr (!std::is_same_v<BoundT, std::monostate>)
            {
                this->set_bound_value_to_default<BoundT>(boundVal);
            }
            else
            {
                throw std::runtime_error{std::string{"The option "} + this->data().longName + " is missing a bound value."};
            }
        };

        std::visit(valueSetter, this->data().boundValue);
    }

    [[nodiscard]] auto set_value(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) const -> bool
    {
        if (this->set_value(this->extract_value(argument, end)))
        {
            ++(*argument);
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    auto advance_argument_iterator(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) -> void
    {
        if (++(*argument) == end)
        {
            throw std::runtime_error{std::string{"Missing value for option '"} + this->data().longName + "'."};
        }
    }

    [[nodiscard]] static auto extract_named_value(const std::string &argument) -> std::string
    {
        const size_t pos = argument.find('=');

        if (pos != std::string::npos)
        {
            return argument.substr(pos + 1);
        }
        else
        {
            return {};
        }
    }

    [[nodiscard]] auto extract_value(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) const -> std::string
    {
        std::string value = this->extract_value(**argument);

        if (!this->has_value(value))
        {
            this->advance_argument_iterator(argument, end);
            return **argument;
        }
        else
        {
            return value;
        }
    }

    [[nodiscard]] auto extract_value(const std::string &argument) const -> std::string
    {
        if (this->is_positional())
        {
            return argument;
        }
        else
        {
            return this->extract_named_value(argument);
        }
    }

    auto handle_set_option_failure([[maybe_unused]] std::exception &e) -> void
    {
        if (!this->is_positional())
        {
            const auto throwError = [&](auto &&boundValue) {
                using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;
                throw std::runtime_error{std::string{"Failed to set option '"} + this->data().longName + "' (" + typeid(BoundT).name() + ") from value '" + value + "'."};
            };

            std::visit(throwError, this->data().boundValue);
        }
    }

    [[nodiscard]] auto has_value(const std::string &value) const -> bool
    {
        return !value.empty() || this->is_switch();
    }

    template<typename T>
    auto set_bound_value_to_default(T *boundVal) const -> void
    {
        const auto valueSetter = [&](auto &&defaultValue) {
            using DefaultT = std::decay_t<decltype(defaultValue)>;

            if constexpr (std::is_same_v<T, DefaultT>)
            {
                *boundVal = defaultValue;
            }
        };

        std::visit(valueSetter, this->data().defaultValue);
    }

    [[nodiscard]] auto set_value(const std::string &value) const -> bool
    {
        const auto valueSetter = [&](auto &&boundValue) {
            using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;

            if constexpr (std::is_same_v<BoundT, std::monostate>)
            {
                throw std::runtime_error{std::string{"Bind value undefined for option '"} + this->long_name() + "'."};
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
                *boundValue = OptionMatcher::unquote(value);
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
                (*boundValue).emplace_back(OptionMatcher::unquote(value));
            }
        };

        try
        {
            std::visit(valueSetter, this->data().boundValue);
            return true;
        }
        catch ([[maybe_unused]] std::runtime_error &e)
        {
            throw;
        }
        catch (std::exception &e)
        {
            this->handle_set_option_failure(e);
        }

        return false;
    }

    [[nodiscard]] static auto is_quoted(const std::string &value) noexcept -> bool
    {
        return value.size() >= 2 && value.front() == '"' && value.back() == '"';
    }

    [[nodiscard]] static auto unquote(const std::string &value) -> std::string
    {
        if (OptionMatcher::is_quoted(value))
        {
            return std::string{value.begin() + 1, value.end() - 1};
        }

        return value;
    }
};
}