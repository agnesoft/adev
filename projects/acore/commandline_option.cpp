#ifdef _MSC_VER
module;

#    include <typeinfo>

export module acore : commandline_option;
import : acore_common;
#endif

namespace acore
{
//! \private
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

//! The CommandLineOption is a builder class that
//! provides fluent interface for configuration of
//! command line options and their matching.
//!
//! The building is started by instantiating the
//! CommandLineOption::Option class (usually via
//! CommandLine object). The available methods then
//! guide the configuration of the option allowing
//! many optional settings where appropriate.
//!
//! The option can be either named or positional.
//! The named options are required to have a long
//! name and a description. Positional arguments
//! require only a description. The description is
//! intentionally made mandatory. All options must
//! also be bound to a value.
//!
//! Optional parameters are:
//!
//! - required: an option must be matched otherwise an error is raised
//! - defaultValue: if an option is not matched set the bound value to this value
//! - shortName: (only for named options) is a single character alternative for a named option
//!
//! Furthermore it is possible to specify a repeated
//! option simply by binding the value to an std::vector
//! of a selected type. Such an option can be matched
//! multiple times and will append all matched values
//! to the vector. Repeated values can be required
//! or defaulted like a regular option.
//!
//! The requirements for the named options are:
//!
//! - long name: `[a-zA-Z][a-zA-Z\\d]+` (a letter of either case followed by at least one or more letter(s) of either case or by digit(s) or combination of both)
//! - short name: `[a-zA-Z]` (a single alphabetic character, either lower or upper case)
//!
//! There is no restrictions placed on positional
//! arguments. The matching is done using the unified
//! syntax:
//!
//! - long name: always prefixed with double dash, e.g. `--longName1`
//! - short name: always prefixed with single dash, e.g. `-I`
//!
//! The value corresponding to the option can be specified
//! in two ways:
//!
//! - equal sign, e.g. `-I=/some/path/`
//! - as following value, `--longName1 value`
//!
//! The quoted values, both named or positional are
//! unquoted, e.g. `"some value with spaces"` will be
//! parsed and saved to the bound value as `some value with spaces`
//! without any quotes.
export class CommandLineOption
{
public:
    //! A variant holding a default value to be bound
    //! if an option is not matched.
    using DefaultValue = std::variant<std::monostate, bool, std::int64_t, double, std::string, std::vector<std::int64_t>, std::vector<double>, std::vector<std::string>>;

    //! A variant holding a pointer to the bound value.
    using BoundValue = std::variant<std::monostate, bool *, std::int64_t *, double *, std::string *, std::vector<std::int64_t> *, std::vector<double> *, std::vector<std::string> *>;

private:
    //! Base class for the command line option
    //! builder classes.
    class Base
    {
    public:
        //! Constructs the option with \a option
        //! as the internal option.
        explicit Base(CommandLineOption *option) noexcept :
            mOption{option}
        {
        }

    protected:
        //! Returns the internal option.
        [[nodiscard]] auto option() const noexcept -> CommandLineOption *
        {
            return mOption;
        }

    private:
        CommandLineOption *mOption = nullptr;
    };

    //! The builder class of the command line
    //! option settomg the bound value to load
    //! the option into during parsing.
    class Described : public Base
    {
    public:
        using Base::Base;

        //! Binds the option to \a value.
        auto bindTo(BoundValue value) -> const CommandLineOption &
        {
            option()->bindTo(value);
            return *option();
        }
    };

    //! The builder class for the command
    //! line option. It represents the
    //! state of a defined option but without
    //! a description (that is required).
    class Defined : public Base
    {
    public:
        using Base::Base;

        //! Sets the option's description to \a desc.
        [[nodiscard]] auto description(std::string desc) noexcept -> Described
        {
            option()->setDescription(std::move(desc));
            return Described{option()};
        }
    };

    //! The builder class of the command line
    //! option allowing defining option's
    //! value.
    class Value : public Base
    {
    public:
        using Base::Base;

        //! Sets the default value to be \a defaultValue.
        //! The value will be set to the bound value if
        //! no option comes from the command line.
        //! Mutually exclusive with required().
        [[nodiscard]] auto defaultValue(DefaultValue defaultValue) noexcept -> Defined
        {
            option()->setDefaultValue(std::move(defaultValue));
            return Defined{option()};
        }

        //! Sets the option's description to \a description.
        [[nodiscard]] auto description(std::string description) noexcept -> Described
        {
            option()->setDescription(std::move(description));
            return Described{option()};
        }

        //! Sets this option as being required. Mutually
        //! exclusive with defaultValue().
        [[nodiscard]] auto required() noexcept -> Defined
        {
            option()->setRequired();
            return Defined{option()};
        }
    };

    //! The builder class of the command line
    //! option setting the short name of the
    //! option.
    class Named : public Value
    {
    public:
        using Value::Value;

        //! Sets the short name of the option to
        //! \a shortName.
        [[nodiscard]] auto shortName(char shortName) -> Value
        {
            option()->setShortName(shortName);
            return Value{option()};
        }
    };

public:
    //! The builder class for the command
    //! line option. It is the first entry
    //! point for defining an option. Obtain
    //! the instance of this class by calling
    //! the  CommandLine::option() method.
    class Option : public Base
    {
    public:
        using Base::Base;

        //! Sets the long name of the option
        //! to \a name.
        [[nodiscard]] auto longName(std::string name) -> Named
        {
            option()->setLongName(std::move(name));
            return Named{option()};
        }

        //! Sets the option as positional (having no name).
        [[nodiscard]] auto positional() noexcept -> Value
        {
            return Value{option()};
        }
    };

    //! Returns the currently bound value.
    [[nodiscard]] auto boundValue() const noexcept -> const BoundValue &
    {
        return mBoundValue;
    }

    //! Returns the type of the currently bound value
    //! as string to be displayed in the help. E.g.
    //! bool * -> "switch"
    //! std::int64_t -> "integer"
    //! std::vector<std::string> -> "string list"
    [[nodiscard]] auto boundValueAsString() const -> std::string
    {
        std::stringstream stream;

        std::visit([&](auto &&value) {
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
        },
                   boundValue());

        return stream.str();
    }

    //! Sets the bound value to the default value.
    //! If either of the two was not set this
    //! method does nothing.
    auto defaultBoundValue() const -> void
    {
        if (isDefaulted())
        {
            std::visit([&](auto &&boundVal) {
                using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundVal)>>;

                if constexpr (!std::is_same_v<BoundT, std::monostate>)
                {
                    setBoundValueToDefault<BoundT>(boundVal);
                }
                else
                {
                    throw std::runtime_error{std::string{"The option "} + name() + " is missing a bound value."};
                }
            },
                       boundValue());
        }
    }

    //! Return the currently set default value.
    [[nodiscard]] auto defaultValue() const noexcept -> const DefaultValue &
    {
        return mDefaultValue;
    }

    //! Returns the currently set default value as
    //! string (e.g. as if printed to std::cout).
    [[nodiscard]] auto defaultValueAsString() const -> std::string
    {
        std::ostringstream stream;

        std::visit([&](auto &&value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, bool>)
            {
                stream << (value ? "true" : "false");
            }
            else if constexpr (!std::is_same_v<T, std::monostate>)
            {
                stream << value;
            }
        },
                   defaultValue());

        return stream.str();
    }

    //! Returns the option's description.
    [[nodiscard]] auto description() const noexcept -> const std::string &
    {
        return mDescription;
    }

    //! Returns true if there is a default value set.
    [[nodiscard]] auto isDefaulted() const noexcept -> bool
    {
        return !std::holds_alternative<std::monostate>(defaultValue());
    }

    //! Returns true if this is a positional option.
    [[nodiscard]] auto isPositional() const noexcept -> bool
    {
        return mLongName.empty();
    }

    //! Returns true if the option can be repeated.
    //! I.e. it is bound to std::vector value.
    [[nodiscard]] auto isRepeated() const noexcept -> bool
    {
        return std::holds_alternative<std::vector<std::int64_t> *>(boundValue())
            || std::holds_alternative<std::vector<double> *>(boundValue())
            || std::holds_alternative<std::vector<std::string> *>(boundValue());
    }

    //! Returns true if the option is required.
    [[nodiscard]] auto isRequired() const noexcept -> bool
    {
        return mRequired;
    }

    //! Returns true of the option is a switch. I.e.
    //! it is bound to \c bool.
    [[nodiscard]] constexpr auto isSwitch() const noexcept -> bool
    {
        return std::holds_alternative<bool *>(mBoundValue);
    }

    //! Returns the option's long name. The string
    //! is empty for positional options.
    [[nodiscard]] auto longName() const noexcept -> const std::string &
    {
        return mLongName;
    }

    //! Matches the option against \a argument and
    //! advances it accordingly. If the option was
    //! matched including the value this method
    //! returns true and the \a argument is advanced
    //! by 1. If the option was matched and the value
    //! was taken from the next argument the function
    //! returns true and the \a argument is advanced
    //! by 2. The \a end is used as a guard. If the
    //! option is not matched the method returns
    //! false and the \a argument remains unchanged.
    [[nodiscard]] auto match(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) const -> bool
    {
        return matchOption(extractName(**argument)) && setValue(argument, end);
    }

    //! Returns the name of the option. It is either
    //! the long name or "[positional]" for positional
    //! options.
    [[nodiscard]] auto name() const -> std::string
    {
        if (longName().empty())
        {
            return "[positional]";
        }

        return longName();
    }

    //! Returns the short name of the option. The
    //! value is 0 if the short name was not set
    //! or the option is positional.
    [[nodiscard]] auto shortName() const noexcept -> char
    {
        return mShortName;
    }

private:
    template<typename T>
    auto setBoundValueToDefault(T *boundVal) const -> void
    {
        std::visit([&](auto &&defVal) {
            using DefaultT = std::decay_t<decltype(defVal)>;

            if constexpr (std::is_same_v<T, DefaultT>)
            {
                *boundVal = defVal;
            }
        },
                   defaultValue());
    }

    template<typename T>
    auto validateDefaultTypeCompatibility() const
    {
        std::visit([&](auto &&defVal) {
            using DefaultT = std::decay_t<decltype(defVal)>;

            if constexpr (!std::is_same_v<DefaultT, std::monostate> && !std::is_same_v<T, DefaultT>)
            {
                throw std::runtime_error{std::string{"The option "} + name() + " default value is set with incompatible type (" + typeid(DefaultT).name() + ") to the one it is being bound to (" + typeid(T).name() + ")."};
            }
        },
                   defaultValue());
    }

    auto bindTo(CommandLineOption::BoundValue value) -> void
    {
        std::visit([&](auto &&boundVal) {
            using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundVal)>>;
            validateDefaultTypeCompatibility<BoundT>();
        },
                   value);

        mBoundValue = value;
    }

    [[nodiscard]] static auto extractLongName(const std::string &argument) -> std::string
    {
        if (argument.size() > 3
            && argument[0] == '-'
            && argument[1] == '-')
        {
            const auto pos = argument.find('=', 3);
            std::string name = argument.substr(2, pos - 2);

            if (isLongName(name))
            {
                return name;
            }
        }

        return {};
    }

    [[nodiscard]] static auto extractName(const std::string &argument) -> std::string
    {
        std::string optionName = extractShortName(argument);

        if (optionName.empty())
        {
            optionName = extractLongName(argument);
        }

        return optionName;
    }

    [[nodiscard]] static auto extractShortName(const std::string &argument) -> std::string
    {
        if (argument.size() >= 2
            && argument[0] == '-'
            && isShortName(static_cast<unsigned char>(argument[1]))
            && (argument.size() == 2 || argument[2] == '='))
        {
            return argument.substr(1, 1);
        }

        return {};
    }

    [[nodiscard]] static auto extractNamedValue(const std::string &argument) -> std::string
    {
        const size_t pos = argument.find('=');

        if (pos != std::string::npos)
        {
            return argument.substr(pos + 1);
        }

        return {};
    }

    [[nodiscard]] auto extractValue(const std::string &argument) const -> std::string
    {
        if (isPositional())
        {
            return argument;
        }

        return extractNamedValue(argument);
    }

    [[nodiscard]] static auto isLongName(const std::string &longName) -> bool
    {
        const auto isNotAlphanumeric = [](char c) {
            return std::isalnum(c) == 0;
        };
        return longName.size() > 1 && isShortName(longName[0]) && std::find_if(++longName.begin() + 1, longName.end(), isNotAlphanumeric) == longName.end();
    }

    [[nodiscard]] static auto isQuoted(const std::string &value) noexcept -> bool
    {
        return value.size() >= 2 && value.front() == '"' && value.back() == '"';
    }

    [[nodiscard]] static auto isShortName(unsigned char shortName) noexcept -> bool
    {
        return std::isalpha(shortName) != 0;
    }

    [[nodiscard]] auto matchOption(const std::string &name) const noexcept -> bool
    {
        return name == longName()
            || (name.size() == 1 && name[0] == shortName())
            || (name.empty() && isPositional());
    }

    auto setDefaultValue(CommandLineOption::DefaultValue value) noexcept -> void
    {
        mDefaultValue = std::move(value);
    }

    auto setDescription(std::string description) noexcept -> void
    {
        mDescription = std::move(description);
    }

    auto setLongName(std::string name) -> void
    {
        if (!isLongName(name))
        {
            throw std::runtime_error{std::string{"Long name '"} + name + "' is not a valid name ([a-zA-Z][a-zA-Z\\d]+)."};
        }

        mLongName = std::move(name);
    }

    auto setRequired() noexcept -> void
    {
        mRequired = true;
    }

    auto setShortName(char name) -> void
    {
        if (!isShortName(name))
        {
            throw std::runtime_error{std::string{"Short name '"} + name + "' is not a valid name ([a-zA-Z])."};
        }

        mShortName = name;
    }

    [[nodiscard]] auto setValue(std::vector<std::string>::const_iterator *argument, std::vector<std::string>::const_iterator end) const -> bool
    {
        std::string value = extractValue(**argument);

        if (value.empty() && !isSwitch())
        {
            if (++(*argument) == end)
            {
                throw std::runtime_error{std::string{"Missing value for option '"} + longName() + "'."};
            }

            value = **argument;
        }

        if (setValue(value))
        {
            ++(*argument);
            return true;
        }

        return false;
    }

    [[nodiscard]] auto setValue(const std::string &value) const -> bool
    {
        try
        {
            std::visit([&](auto &&boundValue) {
                using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;

                if constexpr (std::is_same_v<BoundT, std::monostate>)
                {
                    throw std::runtime_error{std::string{"Bind value undefined for option '"} + name() + "'."};
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
                    *boundValue = unquote(value);
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
                    (*boundValue).emplace_back(unquote(value));
                }
            },
                       boundValue());

            return true;
        }
        catch ([[maybe_unused]] std::runtime_error &e)
        {
            throw;
        }
        catch ([[maybe_unused]] std::exception &e)
        {
            if (!isPositional())
            {
                std::visit([&](auto &&boundValue) {
                    using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;

                    throw std::runtime_error{std::string{"Failed to set option '"} + name() + "' (" + typeid(BoundT).name() + ") from value '" + value + "'."};
                },
                           boundValue());
            }
        }

        return false;
    }

    [[nodiscard]] static auto unquote(const std::string &value) -> std::string
    {
        if (isQuoted(value))
        {
            return std::string{value.begin() + 1, value.end() - 1};
        }

        return value;
    }

    std::string mLongName;
    std::string mDescription;
    CommandLineOption::DefaultValue mDefaultValue;
    CommandLineOption::BoundValue mBoundValue;
    bool mRequired = false;
    char mShortName = {};
};
}
