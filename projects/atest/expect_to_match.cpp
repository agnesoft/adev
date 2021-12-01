#ifndef __clang__
export module atest : expect_to_match;
import : expect_base;
import : stringify;
#endif

namespace atest
{
//! The ExpectToMatch<ExpressionT, ValueT,
//! MatcherT, ExpectationType, ResultHandlingPolicy>
//! provides generic value matching expectation.
//!
//! The type `ExpressionT` can be either a value
//! or a callable. If the value is a callable it
//! will be called before the matching takes
//! place. No exceptions are propagated from the
//! call. The `MatcherT`'s `operator()` is used to
//! perform the actual matching. If there is the
//! match the expectation passes (observing the
//! ResultHandlingPolicy). If there is not a match the
//! `MatcherT` will be further queried for details
//! using the static method interface of the
//! MatcherBase class.
export template<typename ExpressionT, typename ValueT, typename MatcherT, ExpectationType expectationType, ResultHandlingPolicy resultHandlingPolicy>
class ExpectToMatch : public ExpectBase<ExpressionT, expectationType, resultHandlingPolicy>
{
public:
    //! Constructs the object with the
    //! `expression`, `value` and `sourceLocation`
    //! and runs the matching. If the
    //! `ExpressionT` is a callable type it will
    //! first be called and only its result will
    //! be compared against value `ValueT`. No
    //! exceptions are propagated from such call.
    //! The matching is done by calling the
    //! `MatcherT::operator()`. If the match fails
    //! `MatcherT` instance will be queried for
    //! the details regarding the match and
    //! provided with the values to use for
    //! stringification of the error output.
    ExpectToMatch(const ExpressionT &expression, const ValueT &value, const std::source_location &sourceLocation) :
        ExpectBase<ExpressionT, expectationType, resultHandlingPolicy>{expression, sourceLocation},
        expected{value}
    {
        try
        {
            this->match(evaluate_expression());
        }
        catch ([[maybe_unused]] const FailedAssertion &exception)
        {
            throw;
        }
        catch (const std::exception &exception)
        {
            this->fail(Failure{.what = ::atest::stringify("Unexpected exception thrown (", typeid(exception).name(), "): ", exception.what())});
        }
        catch (...)
        {
            this->fail(Failure{.what = "Unexpected exception thrown"});
        }
    }

private:
    [[nodiscard]] auto evaluate_expression() const -> auto
    {
        if constexpr (std::is_invocable<ExpressionT>::value)
        {
            return this->expression()();
        }
        else
        {
            return this->expression();
        }
    }

    auto match(const auto &actual) -> void
    {
        MatcherT matcher;

        if (matcher(actual, this->expected))
        {
            this->handle_success();
        }
        else
        {
            this->handle_failure(Failure{.what = matcher.describe(),
                                         .expected = matcher.expected(actual, this->expected),
                                         .actual = matcher.actual(actual, this->expected),
                                         .hint = matcher.hint(actual, this->expected)});
        }
    }

    const ValueT &expected;
};
}
