#ifndef __clang__
module acommandline : option_builder_value;
import : option_builder_defined;
#endif

namespace acommandline
{
//! The OptionBuilderValue class allows defining
//! optional features of a command line option.
class OptionBuilderValue : public OptionBuilderDefined
{
public:
    using OptionBuilderDefined::OptionBuilderDefined;

    //! Sets the default value of the option to
    //! `value`. The default value will be used if
    //! the option does not match any of the
    //! command line arguments.
    //!
    //! It is mutually exclusive with required().
    [[nodiscard]] auto default_value(DefaultValue value) noexcept -> OptionBuilderDefined
    {
        this->option().defaultValue = std::move(value);
        return OptionBuilderDefined{this->option()};
    }

    //! Sets the option as required meaning it
    //! must be matched against at least one
    //! command line arguments.
    //!
    //! It is mutually exclusive with default_value().
    [[nodiscard]] auto required() noexcept -> OptionBuilderDefined
    {
        this->option().required = true;
        return OptionBuilderDefined{this->option()};
    }
};
}
