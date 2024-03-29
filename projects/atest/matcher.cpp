#ifndef __clang__
export module atest : matcher;
import : matcher_base;
#endif

namespace atest
{
//! The Matcher class is the implementation of an
//! atest matcher. It uses `operator==` to match
//! the values.
export class Matcher : public MatcherBase
{
public:
    //! Returns `true` if the `left == right`.
    template<typename LeftValueT, typename RightValueT>
    [[nodiscard]] auto operator()(const LeftValueT &left, const RightValueT &right) const -> bool
    {
        return left == right;
    }
};
}
