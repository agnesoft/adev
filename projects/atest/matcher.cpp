#ifdef _MSC_VER
export module atest : matcher;

export import : stringify;
#endif

namespace atest
{
export class MatcherBase
{
public:
    [[nodiscard]] static auto describe() -> std::string
    {
        return "Values do not match.";
    }

    template<typename T, typename V>
    [[nodiscard]] auto expected(const T &left, [[maybe_unused]] const V &right) const -> std::string
    {
        return stringify(left);
    }

    template<typename T, typename V>
    [[nodiscard]] auto actual([[maybe_unused]] const T &left, const V &right) const -> std::string
    {
        return stringify(right);
    }
};

export class Matcher : public MatcherBase
{
public:
    template<typename T, typename V>
    [[nodiscard]] auto operator()(const T &left, const V &right) const -> bool
    {
        return left == right;
    }
};
}
