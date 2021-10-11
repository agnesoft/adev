#ifndef __clang__
export module atest : expect;
export import : expect_to_match;
export import : expect_to_throw;
export import : matcher;
export import : matcher_contains;
#endif

namespace atest
{
//! The Expect<ExpressionT, ExpectationType,
//! ResultHandlingPolicy> provides the builder for the
//! test expectation or assertion.
//!
//! The class will take any `ExpressionT` that is
//! value or a callable. Selected expectation may
//! impose further requirements on `ExpressionT`
//! such as having certain comparison operator,
//! streaming operator or being a callable.
export template<typename ExpressionT, ExpectationType expectationType, ResultHandlingPolicy resultHandlingPolicy>
class Expect
{
public:
    //! Constructor.
    Expect(const ExpressionT &expression, const std::source_location &sourceLocation) noexcept :
        expression{expression},
        sourceLocation{sourceLocation}
    {
    }

    //! Creates the value matcher expectation with
    //! a default Matcher that uses `operator==`.
    template<typename ValueT>
    auto to_be(const ValueT &value) -> ExpectToMatch<ExpressionT,
                                                     ValueT,
                                                     Matcher,
                                                     expectationType,
                                                     resultHandlingPolicy>
    {
        return {this->expression, value, this->sourceLocation};
    }

    //! Creates the value matcher expectation that
    //! tries to find the `value` in the
    //! `expression`.
    template<typename ValueT>
    auto to_contain(const ValueT &value) -> ExpectToMatch<ExpressionT,
                                                          ValueT,
                                                          MatcherContains,
                                                          expectationType,
                                                          resultHandlingPolicy>
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
                                                        resultHandlingPolicy>
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
                                     resultHandlingPolicy>
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
                                                                resultHandlingPolicy>
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
                                                        resultHandlingPolicy>
    {
        return {this->expression, value, this->sourceLocation};
    }

private:
    const ExpressionT &expression;
    std::source_location sourceLocation;
};
}
