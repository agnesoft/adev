#ifndef __clang__
export module atest : matcher_base;
export import : stringify;
#endif

namespace atest
{
//! The `MatcherBase` is the base class for atest
//! matchers. Matchers are typically used as
//! template arguments of the
//! Expect::to_match() method.
export class MatcherBase
{
public:
    //! Returns the actual value received. The
    //! default implementation returns stringified
    //! `actualValue`.
    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] static auto actual(const ActualValueT &actualValue, [[maybe_unused]] const ExpectedValueT &expectedValue) -> std::string
    {
        return ::atest::stringify(actualValue);
    }

    //! Returns the description of the operation
    //! the matcher performs.
    [[nodiscard]] static auto describe() -> std::string
    {
        return "Values do not match.";
    }

    //! Returns the expected value. The default
    //! implementation returns stringified `expectedValue`.
    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] static auto expected([[maybe_unused]] const ActualValueT &actualValue, const ExpectedValueT &expectedValue) -> std::string
    {
        return ::atest::stringify(expectedValue);
    }

    //! Returns optional hint on the `actualValue`.
    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] static auto hint([[maybe_unused]] const ActualValueT &actualValue, [[maybe_unused]] const ExpectedValueT &expectedValue) -> std::string
    {
        return {};
    }
};
}
