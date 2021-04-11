#ifdef _MSC_VER
export module atest : expect;

export import : expect_tomatch;
export import : expect_tothrow;
export import : matcher;
#endif

namespace atest
{
//! \brief The `Expect<T>` provides the builder for the test expectation (assertion).
//!
//! The class will take any value as `const &T` which may be a value type or callable
//! type. Individual expectations however may impose additional requirements and
//! incompatible calls will result in a compiler error.
export template<typename T>
class Expect
{
public:
    //! Constructs the object with `expression` and `sourceLocation`.
    Expect(const T &expression, const source_location<> &sourceLocation) noexcept :
        mExpression{expression},
        mSourceLocation{sourceLocation}
    {
    }

    //! Creates the matcher expectation with a default Matcher that uses `operator==`.
    template<typename V>
    auto toBe(const V &value) -> ExpectToMatch<T, V, Matcher>
    {
        return ExpectToMatch<T, V, Matcher>{mExpression, value, mSourceLocation};
    }

    //! Creates a generic matcher expectation using a custom matcher `M`.
    template<typename M, typename V>
    auto toMatch(const V &value) -> ExpectToMatch<T, V, M>
    {
        return ExpectToMatch<T, V, M>{mExpression, value, mSourceLocation};
    }

    //! Creates an expectation that expects a type `E` to be thrown from the
    //! callable expression `T`.
    template<typename E>
    requires std::invocable<T> auto toThrow() -> ExpectToThrow<T, E, const char *, false>
    {
        return ExpectToThrow<T, E, const char *, false>{mExpression, "", mSourceLocation};
    }

    //! Creates an expectation that expects a type `E` to be thrown from the callable
    //! expression `T` that matches the object `exception`.
    template<typename E>
    requires std::invocable<T> auto toThrow(const E &exception) -> ExpectToThrow<T, E, E, true>
    {
        return ExpectToThrow<T, E, E, true>{mExpression, exception, mSourceLocation};
    }

    //! Creates an expectation that expects a type `E` to be thrown from the callable
    //! expression `T` that matches the `value`. This overload is typically used for matching
    //! an exception's text.
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