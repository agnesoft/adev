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
    //! Returns the description of the operation
    //! the matcher performs.
    [[nodiscard]] auto describe() const -> std::string
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
