#ifndef __clang__
module acommandline : option_builder_base;
import : option_data;
#endif

namespace acommandline
{
class OptionBuilderBase
{
public:
    explicit OptionBase(OptionData &data) noexcept :
        optionData{data}
    {
    }

protected:
    [[nodiscard]] auto data() const noexcept -> OptionData &
    {
        return this->optionData;
    }

    [[nodiscard]] static auto is_long_name(const std::string &longName) -> bool
    {
        const auto isNotAlphanumeric = [](char c) {
            return std::isalnum(c) == 0;
        };

        return longName.size() > 1
            && OptionBase::is_short_name(longName[0])
            && std::find_if(++longName.begin() + 1, longName.end(), isNotAlphanumeric) == longName.end();
    }

    [[nodiscard]] constexpr auto is_defaulted() const noexcept -> bool
    {
        return !std::holds_alternative<std::monostate>(this->optionData.defaultValue);
    }

    [[nodiscard]] auto is_positional() const noexcept -> bool
    {
        return this->optionData.longName == "[positional]";
    }

    [[nodiscard]] static auto is_short_name(unsigned char shortName) noexcept -> bool
    {
        return std::isalpha(shortName) != 0;
    }

    [[nodiscard]] constexpr auto is_switch() const noexcept -> bool
    {
        return std::holds_alternative<bool *>(this->optionData.boundValue);
    }

private:
    OptionData &optionData;
};
}
