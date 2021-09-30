#ifndef __clang__
export module acommandline : option_builder_value;
export import : option_builder_defined;
#endif

namespace acommandline
{
//! The OptionBuilderValue class allows defining
//! optional features of a command line option.
export class OptionBuilderValue : public OptionBuilderDefined
{
public:
    using OptionBuilderDefined::OptionBuilderDefined;

    //! Sets the default value of the option to
    //! `value`. The default value will be used if
    //! the option does not match any of the
    //! command line arguments.
    //!
    //! It is mutually exclusive with required().
    [[nodiscard]] auto default_value(DefaultValueArg value) -> OptionBuilderDefined
    {
        const auto setter = [&](auto &&boundValue) {
            this->option().defaultValue = boundValue;
        };
        std::visit(setter, value);
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
