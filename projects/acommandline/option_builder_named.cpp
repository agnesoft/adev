#ifndef __clang__
module acommandline : option_builder_named;
import : option_builder_value;
#endif

namespace acommandline
{
class OptionBuilderNamed : protected OptionBuilderValue
{
public:
    using OptionBuilderValue::OptionBuilderValue;

    [[nodiscard]] auto short_name(char shortName) -> OptionBuilderValue
    {
        this->set_short_name(shortName);
        return OptionBuilderValue{this->data()};
    }

private:
    auto set_short_name(char name) -> void
    {
        if (!OptionBuilderBase::is_short_name(name))
        {
            throw std::runtime_error{std::string{"'"} + name + "' is not a valid option short name ([a-zA-Z])."};
        }

        this->data().shortName = name;
    }
};
}
