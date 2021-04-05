export module atest : expect;

export import : expect_tomatch;
export import : expect_tothrow;
export import : matcher;

namespace atest
{
export template<typename T>
class Expect
{
public:
    Expect(const T &expression, const source_location<> &sourceLocation) noexcept :
        mExpression{expression},
        mSourceLocation{sourceLocation}
    {
    }

    template<typename V>
    auto toBe(const V &value) -> ExpectToMatch<T, V, Matcher>
    {
        return ExpectToMatch<T, V, Matcher>{mExpression, value, mSourceLocation};
    }

    template<typename M, typename V>
    auto toMatch(const V &value) -> ExpectToMatch<T, V, M>
    {
        return ExpectToMatch<T, V, M>{mExpression, value, mSourceLocation};
    }

    template<typename E>
    requires std::invocable<T> auto toThrow() -> ExpectToThrow<T, E, int, false>
    {
        return ExpectToThrow<T, E, int, false>{mExpression, 0, mSourceLocation};
    }

    template<typename E>
    requires std::invocable<T> auto toThrow(const E &exception) -> ExpectToThrow<T, E, E, true>
    {
        return ExpectToThrow<T, E, E, true>{mExpression, exception, mSourceLocation};
    }

    template<typename E, typename V>
    requires std::invocable<T> auto toThrow(const V &value) -> ExpectToThrow<T, E, V, true>
    {
        return ExpectToThrow<T, E, V, true>{mExpression, value, mSourceLocation};
    }

private:
    const T &mExpression;
    source_location<> mSourceLocation;
};
}
