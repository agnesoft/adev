#ifndef __clang__
export module acommandline:option_builder;
export import :option_builder_named;
#endif

namespace acommandline
{
//! The OptionBuilder class represents the first
//! step in composing a command line option.
export class OptionBuilder : public OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    //! Sets the long name of the option to `name`
    //! making the option a named option. The
    //! `name` must:
    //!
    //! - NOT contain any prefix such as `--`
    //! - be at least 2 characters long
    //! - use only:
    //!     - alphanumeric characters
    //!     - underscores: `_`
    //!     - dash: `-`
    //!     - dot: `.`
    //! - start with a letter
    [[nodiscard]] auto long_name(std::string name) -> OptionBuilderNamed
    {
        this->set_long_name(std::move(name));
        return OptionBuilderNamed{this->option()};
    }

    //! Sets the option as a positional (nameless)
    //! option.
    [[nodiscard]] auto positional() -> OptionBuilderValue
    {
        this->option().longName = ::acommandline::POSITIONAL_LONG_NAME;
        return OptionBuilderValue{this->option()};
    }

private:
    auto set_long_name(std::string name) -> void
    {
        if (!::acommandline::is_long_name(name))
        {
            throw std::logic_error{"'" + name + "' is not a valid option long name ([a-zA-Z][a-zA-Z\\d_-\\.]+)."};
        }

        this->option().longName = std::move(name);
    }
};
}
