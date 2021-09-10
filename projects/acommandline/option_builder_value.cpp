#ifndef __clang__
module acommandline : option_builder_value;
import : option_builder_defined;
#endif

namespace acommandline
{
class OptionBuilderValue : public OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    [[nodiscard]] auto default_value(DefaultValue value) noexcept -> OptionBuilderDefined
    {
        this->data().defaultValue = std::move(value);
        return OptionBuilderDefined{this->data()};
    }

    [[nodiscard]] auto description(std::string desc) noexcept -> OptionBuilderDescribed
    {
        this->data().description = std::move(desc);
        return OptionBuilderDescribed{this->data()};
    }

    [[nodiscard]] auto required() noexcept -> OptionBuilderDefined
    {
        this->data().required = true;
        return OptionBuilderDefined{this->data()};
    }
};
}
