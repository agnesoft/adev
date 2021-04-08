#ifdef _MSC_VER
export module atest : matcher;

export import : stringify;
#endif

namespace atest
{
//! The `MatcherBase` is the base class for matchers to be supplied to Expect::toMatch().
export class MatcherBase
{
public:
    //! Returns the description of the operation the matcher performs.
    [[nodiscard]] static auto describe() -> std::string
    {
        return "Values do not match.";
    }

    //! Returns the expected value. The default implementation returns stringified `left`.
    template<typename T, typename V>
    [[nodiscard]] auto expected(const T &left, [[maybe_unused]] const V &right) const -> std::string
    {
        return stringify(left);
    }

    //! Returnst the actual value recieved. The default implementation returns stringified `right`.
    template<typename T, typename V>
    [[nodiscard]] auto actual([[maybe_unused]] const T &left, const V &right) const -> std::string
    {
        return stringify(right);
    }
};

//! The concrete implementation of the Matcher that uses `operator==`.
export class Matcher : public MatcherBase
{
public:
    //! Returns `true` if the `left == right`.
    template<typename T, typename V>
    [[nodiscard]] auto operator()(const T &left, const V &right) const -> bool
    {
        return left == right;
    }
};
}
