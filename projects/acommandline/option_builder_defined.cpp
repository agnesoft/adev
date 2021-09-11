#ifndef __clang__
module acommandline : option_builder_defined;
import : option_builder_described;
#endif

namespace acommandline
{
class OptionBuilderDefined : public OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    [[nodiscard]] auto description(std::string description) noexcept -> OptionBuilderDescribed
    {
        this->option().description = std::move(description);
        return OptionBuilderDescribed{this->option()};
    }
};
}
