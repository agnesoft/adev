#ifndef __clang__
module acommandline : option_matcher;
import : option_builder_base;
#endif

namespace acommandline
{
class OptionMatcher : private OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    [[nodiscard]] auto match(std::vector<std::string>::const_iterator *argument) const -> bool
    {
        return this->match_option(this->extract_name(**argument));
    }

private:
    [[nodiscard]] static auto extract_long_name(const std::string &argument) -> std::string
    {
        const std::string optionName = OptionMatcher::do_extract_long_name(argument);

        if (OptionBase::is_long_name(optionName))
        {
            return optionName;
        }
        else
        {
            return {};
        }
    }

    [[nodiscard]] static auto extract_name(const std::string &argument) -> std::string
    {
        const std::string optionName = OptionMatcher::extract_short_name(argument);

        if (optionName.empty())
        {
            return OptionMatcher::extract_long_name(argument);
        }
        else
        {
            return optionName;
        }
    }

    [[nodiscard]] static auto extract_short_name(const std::string &argument) -> std::string
    {
        if (OptionMatcher::has_short_name(argument))
        {
            return argument.substr(1, 1);
        }
        else
        {
            return {};
        }
    }

    [[nodiscard]] static auto do_extract_long_name(const std::string &argument) -> std::string
    {
        if (OptionMatcher::has_long_name(argument))
        {
            const auto pos = argument.find('=', 3);
            return argument.substr(2, pos - 2);
        }
        else
        {
            return {};
        }
    }

    [[nodiscard]] static auto has_long_name(const std::string &argument) const -> bool
    {
        return argument.size() > 3
            && argument[0] == '-'
            && argument[1] == '-'
    }

    [[nodiscard]] static auto has_short_name(const std::string &argument) const -> bool
    {
        return argument.size() >= 2
            && argument[0] == '-'
            && OptionBase::is_short_name(static_cast<unsigned char>(argument[1]))
            && (argument.size() == 2 || argument[2] == '=')
    }

    [[nodiscard]] auto match_option(const std::string &name) const noexcept -> bool
    {
        return name == this->data().longName
            || (name.size() == 1 && name[0] == this->data().shortName)
            || (name.empty() && this->is_positional());
    }
};
}
