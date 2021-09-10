#ifndef __clang__
module acommandline : option_builder_defined;
import : option_builder_described;
#endif

namespace acommandline
{
class OptionBuilderDefined : protected OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    [[nodiscard]] auto description(std::string description) noexcept -> OptionBuilderDescribed
    {
        this->data().description = std::move(description);
        return {this->data()};
    }
};
}
