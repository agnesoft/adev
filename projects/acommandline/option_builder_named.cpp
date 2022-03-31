#ifndef __clang__
export module acommandline:option_builder_named;
export import :option_builder_value;
#endif

namespace acommandline
{
//! The OptionBuilderNamed continues building a
//! named command line option.
export class OptionBuilderNamed : public OptionBuilderValue
{
public:
    using OptionBuilderValue::OptionBuilderValue;

    //! Sets the short name of the option to `name`. The `name` must:
    //!
    //! - be a lower or upper case letter
    [[nodiscard]] auto short_name(char name) -> OptionBuilderValue
    {
        this->set_short_name(name);
        return OptionBuilderValue{this->option()};
    }

private:
    auto set_short_name(char name) -> void
    {
        if (!::acommandline::is_short_name(name))
        {
            throw std::logic_error{std::string{"'"} + name + "' is not a valid option short name ([a-zA-Z])."};
        }

        this->option().shortName = name;
    }
};
}
