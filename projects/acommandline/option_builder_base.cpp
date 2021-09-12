#ifndef __clang__
module acommandline : option_builder_base;
import : option;
#endif

namespace acommandline
{
//! \private
class OptionBuilderBase
{
public:
    explicit OptionBuilderBase(Option &data) noexcept :
        optionData{data}
    {
    }

protected:
    //! \private
    [[nodiscard]] auto option() noexcept -> Option &
    {
        return this->optionData;
    }

    //! \private
    [[nodiscard]] auto option() const noexcept -> const Option &
    {
        return this->optionData;
    }

private:
    Option &optionData;
};
}
