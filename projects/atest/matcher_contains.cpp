#ifndef __clang__
export module atest:matcher_contains;
import :matcher_base;
#endif

namespace atest
{
//! The MatcherContains class is the
//! implementation of an atest matcher. It uses
//! `find` algorithm to match the values.
export class MatcherContains : public MatcherBase
{
public:
    //! Returns the description of the operation
    //! the matcher performs.
    [[nodiscard]] static auto describe() -> std::string
    {
        return "Expected is not contained in Actual";
    }

    //! Returns `true` if `right` is found int
    //! `left`.
    template<typename ActualValueT, typename ExpectedValueT>
    [[nodiscard]] auto operator()(const ActualValueT &actualValue, const ExpectedValueT &expectedValue) const -> bool
    {
        if constexpr (StringConvertible<ActualValueT>)
        {
            return ::atest::stringify(actualValue).find(expectedValue) != std::string::npos;
        }
        else
        {
            return std::find(actualValue.begin(), actualValue.end(), expectedValue) != actualValue.end();
        }
    }
};
}
