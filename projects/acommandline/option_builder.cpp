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
        return OptionBuilderNamed{this->option()};
    }

    [[nodiscard]] auto positional() noexcept -> OptionBuilderValue
    {
        this->option().longName = ::acommandline::POSITIONAL_LONG_NAME;
        return OptionBuilderValue{this->option()};
    }

private:
    auto set_long_name(std::string name) -> void
    {
        if (!::acommandline::is_long_name(name))
        {
            throw std::runtime_error{std::string{"'"} + name + "' is not a valid option long name ([a-zA-Z][a-zA-Z\\d]+)."};
        }

        this->option().longName = std::move(name);
    }
};
}
