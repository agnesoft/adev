#ifdef _MSC_VER
export module atest : expect_tomatch;
export import : expect_base;
#endif

namespace atest
{
//! \brief The `ExpectToMatch<T, V, Matcher>` provides generic value
//! matching expectation.
//!
//! The type `T` can be either a value or a callable. If the value is
//! a callable it will be called before the matching takes place. No
//! exceptions are propagated from the call. The `Matcher`'s `operator()`
//! is used to perform the actual matching. If there is the match the
//! expectation passes. If there is not a match the `Matcher` will be further
//! querried for details.
//!
//! This class is primarily designed to work with a `matcher` derived from MatcherBase
//! such as Matcher but can be used with any type that implements the same interface.
export template<typename T, typename V, typename Matcher, bool Assert, bool ExpectFail>
class ExpectToMatch : public ExpectBase<T, Assert, ExpectFail>
{
public:
    //! Constructs the object with the `expression`, `value` and `sourceLocation` a
    //! and runs the matching. If `T` is a callable type it will first be called and
    //! only its result will be compared against value `V`. No exceptions are propagated
    //! from such call. The matching is done by calling the `operator()` of the `Matcher`.
    //! If the match fails `Matcher` will be queried for the details regarding the match
    //! and provided with the values to possibly stringify for the error output.
    ExpectToMatch(const T &expression, const V &value, const std::source_location &sourceLocation) :
        ExpectBase<T, Assert, ExpectFail>{expression, sourceLocation},
        mValue{value}
    {
        Matcher matcher;

        try
        {
            const auto left = evaluateExpression();

            if (matcher(left, mValue))
            {
                this->handleSuccess();
            }
            else
            {
                this->handleFailure(Failure{matcher.describe(), matcher.expected(left, mValue), matcher.actual(left, mValue)});
            }
        }
        catch ([[maybe_unused]] FailedAssertion &e)
        {
            throw;
        }
        catch (std::exception &e)
        {
            this->fail(Failure{stringify("Unexpected exception thrown (", typeid(e).name(), "): ", e.what())});
        }
        catch (...)
        {
            this->fail(Failure{"Unexpected exception thrown"});
        }
    }

private:
    [[nodiscard]] auto evaluateExpression() const -> auto
    {
        if constexpr (std::is_invocable<T>::value)
        {
            return this->expression()();
        }
        else
        {
            return this->expression();
        }
    }

    const V &mValue;
};
}
