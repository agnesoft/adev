#ifndef __clang__
export module atest : matcher_contains;
import : matcher_base;
#endif

namespace atest
{
//! The MatcherContains class is the
//! implementation of an atest matcher. It uses
//! `find` algorithm to match the values.
export class MatcherContains : public atest::MatcherBase
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
    template<typename LeftValueT, typename RightValueT>
    [[nodiscard]] auto operator()(const LeftValueT &left, const RightValueT &right) const -> bool
    {
        if constexpr (StringConvertible<LeftValueT>)
        {
            return std::string{left}.find(right) != std::string::npos;
        }
        else
        {
            return std::find(left.begin(), left.end(), right) != left.end();
        }
    }
};
}
