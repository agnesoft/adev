#ifndef __clang__
module acommandline : option_builder_described;
import : option_builder_base;
#endif

namespace acommandline
{
class OptionBuilderDescribed : protected OptionBuilderBase
{
public:
    using OptionBuilderBase::OptionBuilderBase;

    auto bind_to(BoundValue value) -> void
    {
        this->bind(std::move(value));
    }

private:
    auto bind(BoundValue value) -> void
    {
        const auto validator = [&](auto &&boundValue) {
            using BoundT = std::remove_pointer_t<std::decay_t<decltype(boundValue)>>;
            this->validate_default_type_compatibility<BoundT>();
        };

        std::visit(validator, value);
        this->data().boundValue = std::move(value);
    }

    template<typename T>
    auto validate_default_type_compatibility() const
    {
        const auto validator = [&](auto &&defaultValue) {
            using DefaultT = std::decay_t<decltype(defaultValue)>;

            if constexpr (!std::is_same_v<DefaultT, std::monostate> && !std::is_same_v<T, DefaultT>)
            {
                throw std::runtime_error{std::string{"The option '"} + this->data().longName + "''s default value is set with incompatible type (" + typeid(DefaultT).name() + ") to the one it is being bound to (" + typeid(T).name() + ")."};
            }
        };

        std::visit(validator, this->data().defaultValue);
    }
};
}
