#ifndef __clang__
export module acommandline : option_builder_defined;
export import : option_builder_described;
#endif

namespace acommandline
{
//! The OptionBuilderDefined class represents a
//! mandatory step in defining the command line
//! option description.
export class OptionBuilderDefined : public OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    //! Sets the option description to
    //! `description`. Setting a description is
    //! mandatory but it can be empty.
    [[nodiscard]] auto description(std::string description) -> OptionBuilderDescribed
    {
        this->option().description = std::move(description);
        return OptionBuilderDescribed{this->option()};
    }
};
}
