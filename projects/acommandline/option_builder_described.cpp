#ifndef __clang__
export module acommandline:option_builder_described;
import :option_builder_base;
#endif

namespace acommandline
{
//! The OptionBuilderDescribed class represents
//! the final step in defining a command line
//! option.
export class OptionBuilderDescribed : public OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    //! Binds the option to `value`. When an
    //! argument matches the option during parsing
    //! its value will be saved to `value`. If the
    //! `value` is a vector of values the parsed
    //! value will be appended. If the option has
    //! default value set the type of `value` must
    //! be compatible with it.
    auto bind_to(BoundValue value) -> void
    {
        this->bind(value);
    }

private:
    template<typename T>
    auto validate_default_type_compatibility() const
    {
        if (this->option().defaultValue.has_value())
        {
            const auto validator = [&](auto &&defaultValue) {
                using DefaultT = std::decay_t<decltype(defaultValue)>;

                if constexpr (!std::is_same_v<T, DefaultT>)
                {
                    throw std::logic_error{"The option '" + this->option().longName + "' default value is set with incompatible type (" + typeid(DefaultT).name() + ") to the one it is being bound to (" + typeid(T).name() + ")."};
                }
            };

            std::visit(validator, *this->option().defaultValue);
        }
    }

    auto bind(BoundValue value) -> void
    {
        const auto validator = [&](auto &&boundValue) {
            using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;
            this->validate_default_type_compatibility<BoundT>();
        };

        std::visit(validator, value);
        this->option().boundValue = value;
    }
};
}
