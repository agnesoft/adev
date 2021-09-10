#ifndef __clang__
module acommandline : option_builder_value;
import : option_builder_defined;
#endif

namespace acommandline
{
class OptionBuilderValue : protected OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    [[nodiscard]] auto default_value(DefaultValue value) noexcept -> OptionBuilderDefined
    {
        this->option().defaultValue = std::move(value);
        return OptionBuilderDefined{this->option()};
    }

    [[nodiscard]] auto description(std::string desc) noexcept -> OptionBuilderDescribed
    {
        this->option().description = std::move(desc);
        return OptionBuilderDescribed{this->option()};
    }

    [[nodiscard]] auto required() noexcept -> OptionBuilderDefined
    {
        this->option().required = true;
        return OptionBuilderDefined{this->option()};
    }
};
}
