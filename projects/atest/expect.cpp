#ifndef __clang__
module atest : expect;
import : expect_to_match;
import : expect_to_throw;
import : matcher;
#endif

namespace atest
{
//! \brief The `Expect<ExpressionT,
//! ExpectationType, FailurePolicy>` provides the
//! builder for the test expectation or assertion.
//!
//! The class will take any value expression or
//! callable `ExpressionT`. Concrete expectation
//! types may impose further requirements on
//! `ExpressionT` such as having certain
//! comparison operator, streaming operator or
//! being a callable.
export template<typename ExpressionT, ExpectationType expectationType, FailurePolicy failurePolicy>
class Expect
{
public:
    //! Constructs the object.
    Expect(const ExpressionT &expression, const std::source_location &sourceLocation) noexcept :
        expression{expression},
        sourceLocation{sourceLocation}
    {
    }

    //! Creates the matcher expectation with a
    //! default Matcher that uses `operator==`.
    template<typename ValueT>
    auto to_be(const ValueT &value) -> ExpectToMatch<ExpressionT,
                                                     ValueT,
                                                     Matcher,
                                                     expectationType,
                                                     failurePolicy>
    {
        return {this->expression, value, this->sourceLocation};
    }

    //! Creates a generic matcher expectation
    //! using a custom matcher `MatcherT`.
    template<typename MatcherT, typename ValueT>
    auto to_match(const ValueT &value) -> ExpectToMatch<ExpressionT,
                                                        ValueT,
                                                        MatcherT,
                                                        expectationType,
                                                        failurePolicy>
    {
        return {this->expression, value, this->sourceLocation};
    }

    //! Creates an expectation that expects a type
    //! `ExceptionT` to be thrown from the
    //! callable expression `ExpressionT`.
    template<typename ExceptionT>
    requires std::invocable<ExpressionT>
    auto to_throw() -> ExpectToThrow<ExpressionT,
                                     ExceptionT,
                                     const char *,
                                     ExceptionValidationPolicy::TypeOnly,
                                     expectationType,
                                     failurePolicy>
    {
        return {this->expression, "", this->sourceLocation};
    }

    //! Creates an expectation that expects a type
    //! `ExceptionT` to be thrown from the
    //! callable expression `ExpressionT` that
    //! matches the object `exception`.
    template<typename ExceptionT>
    requires std::invocable<ExpressionT>
    auto to_throw(const ExceptionT &exception) -> ExpectToThrow<ExpressionT,
                                                                ExceptionT,
                                                                ExceptionT,
                                                                ExceptionValidationPolicy::Value,
                                                                expectationType,
                                                                failurePolicy>
    {
        return {this->expression, exception, this->sourceLocation};
    }

    //! Creates an expectation that expects a type
    //! `ExceptionT` to be thrown from the
    //! callable expression `ExpressionT` that
    //! matches the `value`. This overload is
    //! typically used for matching an exception's
    //! text.
    template<typename ExceptionT, typename ValueT>
    requires std::invocable<ExpressionT>
    auto to_throw(const ValueT &value) -> ExpectToThrow<ExpressionT,
                                                        ExceptionT,
                                                        ValueT,
                                                        ExceptionValidationPolicy::Value,
                                                        expectationType,
                                                        failurePolicy>
    {
        return {this->expression, value, this->sourceLocation};
    }

private:
    const ExpressionT &expression;
    std::source_location sourceLocation;
};
}
