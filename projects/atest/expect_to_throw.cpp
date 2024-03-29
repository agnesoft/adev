#ifndef __clang__
export module atest:expect_to_throw;
import :expect_base;
import :stringify;
#endif

namespace atest
{
//! \private
template<typename T>
concept HasWhat = requires(const T &type)
{
    // clang-format off
    {type.what()} -> std::convertible_to<std::string>;
    // clang-format on
};

//! \private
enum class ExceptionValidationPolicy
{
    TypeOnly,
    Value
};

//! The ExpectToThrow<ExpressionT, ExceptionT,
//! ValueT, ExceptionValidationPolicy,
//! ExpectationType, ResultHandlingPolicy>
//! provides exception related expectation.
//!
//! The type `ExpressionT` must be an
//! `std::invocable` type. Type `ExceptionT` is
//! the type of the exception. Type `ValueT` is
//! the value against which the exception of type
//! `ExceptionT` will be optionally matched. The
//! types are always matched using `typeid`.
//!
//! The type `ValueT` does not necessarily have to
//! be of the same type as `ExceptionT` but there
//! must be an `operator==` that takes both the
//! exception and `ValueT` values. Additionally if
//! the exception `ExceptionT` has `what()` method
//! that returns a type convertible to
//! `std::string` the value `ValueT` will be
//! matched against the `ExceptionT::what()`
//! rather than the exception object itself.
export template<typename ExpressionT, typename ExceptionT, typename ValueT, ExceptionValidationPolicy validationPolicy, ExpectationType expectationType, ResultHandlingPolicy resultHandlingPolicy>
requires std::invocable<ExpressionT>
class ExpectToThrow : public ExpectBase<ExpressionT, expectationType, resultHandlingPolicy>
{
public:
    using ExpectBase<ExpressionT, expectationType, resultHandlingPolicy>::ExpectBase;

    //! Constructs the object taking the reference
    //! to `expression`, `value` and
    //! `sourceLocation`. Invokes `ExpressionT`
    //! expecting the exception of type
    //! `ExceptionT`. If an exception is thrown it
    //! will be validated against the type and
    //! optionally the value. Not throwing any
    //! exception, throwing unknown exception,
    //! throwing an exception of a different type
    //! or with a different value all fail the
    //! expectation.
    ExpectToThrow(const ExpressionT &expression, const ValueT &value, const std::source_location &sourceLocation) :
        ExpectBase<ExpressionT, expectationType, resultHandlingPolicy>{expression, sourceLocation},
        value{value}
    {
        try
        {
            this->expression()();
            this->handle_failure(Failure{.what = "No exception thrown",
                                         .expected = ::atest::stringify(typeid(ExceptionT).name(), " '", this->value, '\'')});
        }
        catch ([[maybe_unused]] const FailedAssertion &exception)
        {
            throw;
        }
        catch (const ExceptionT &exception)
        {
            this->validate_exception(exception);
        }
        catch (...)
        {
            this->handle_unknown_exception();
        }
    }

private:
    auto do_validate_exception_value(const ExceptionT &exception) -> void
    {
        if (this->match_exception_value(exception))
        {
            this->handle_success();
        }
        else
        {
            this->handle_failure(Failure{.what = "Exception value mismatch",
                                         .expected = ::atest::stringify(this->value),
                                         .actual = this->exception_to_string(exception)});
        }
    }

    [[nodiscard]] auto exception_to_string(const ExceptionT &exception) -> std::string
    {
        if constexpr (HasWhat<ExceptionT>)
        {
            return ::atest::stringify(exception.what());
        }
        else
        {
            return ::atest::stringify(exception);
        }
    }

    auto handle_unknown_exception()
    {
        try
        {
            throw;
        }
        catch (const std::exception &exception)
        {
            this->handle_failure(Failure{.what = "Exception type mismatch",
                                         .expected = typeid(ExceptionT).name(),
                                         .actual = typeid(exception).name()});
        }
        catch (...)
        {
            this->handle_failure(Failure{.what = "Exception type mismatch",
                                         .expected = typeid(ExceptionT).name(),
                                         .actual = "Unknown exception"});
        }
    }

    [[nodiscard]] auto match_exception_value(const ExceptionT &exception) -> bool
    {
        if constexpr (HasWhat<ExceptionT>)
        {
            return ::atest::stringify(this->value) == exception.what();
        }
        else
        {
            return this->value == exception;
        }
    }

    auto validate_exception(const ExceptionT &exception) -> void
    {
        if (typeid(ExceptionT) == typeid(exception))
        {
            this->validate_exception_value(exception);
        }
        else
        {
            this->handle_failure(Failure{.what = "Exception type mismatch",
                                         .expected = typeid(ExceptionT).name(),
                                         .actual = typeid(exception).name()});
        }
    }

    auto validate_exception_value(const ExceptionT &exception) -> void
    {
        if constexpr (validationPolicy == ExceptionValidationPolicy::Value)
        {
            this->do_validate_exception_value(exception);
        }
        else
        {
            this->handle_success();
        }
    }

    const ValueT &value;
};
}
