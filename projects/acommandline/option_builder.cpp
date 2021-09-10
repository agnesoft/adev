#ifndef __clang__
module acommandline : option_builder;
import : option_builder_named;
#endif

namespace acommandline
{
class OptionBuilder : public OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    [[nodiscard]] auto long_name(std::string name) -> OptionBuilderNamed
    {
        this->set_long_name(std::move(name));
        return OptionBuilderNamed{this->data()};
    }

    [[nodiscard]] auto positional() noexcept -> OptionBuilderValue
    {
        this->option().longName = "[positional]";
        return OptionBuilderValue{this->data()};
    }

private:
    auto set_long_name(std::string name) -> void
    {
        if (!OptionBuilderBase::is_long_name(name))
        {
            throw std::runtime_error{std::string{"'"} + name + "' is not a valid option long name ([a-zA-Z][a-zA-Z\\d]+)."};
        }

        this->data().longName = std::move(name);
    }
};
}
