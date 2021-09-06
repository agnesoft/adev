#ifndef __clang__
export module atest : matcher_base;
import : stringify;
#endif

namespace atest
{
//! The `MatcherBase` is the base class for
//! matchers to be supplied to Expect::toMatch()
//! call.
export class MatcherBase
{
public:
    //! Returns the description of the operation
    //! the matcher performs.
    [[nodiscard]] static auto describe() -> std::string
    {
        return "Values do not match.";
    }

    //! Returns the expected value. The default
    //! implementation returns stringified `left`.
    template<typename LeftValueT, typename RightValueT>
    [[nodiscard]] auto expected([[maybe_unused]] const LeftValueT &left, const RightValueT &right) const -> std::string
    {
        return ::atest::stringify(right);
    }

    //! Returns the actual value received. The
    //! default implementation returns stringified
    //! `right`.
    template<typename LeftValueT, typename RightValueT>
    [[nodiscard]] auto actual(const LeftValueT &left, [[maybe_unused]] const RightValueT &right) const -> std::string
    {
        return ::atest::stringify(left);
    }
};
}
