#ifndef __clang__
module atest : expect_to_throw;
import : stringify;
import : expect_base;
#else
namespace std
{
template<class _From, class _To>
concept convertible_to =
  is_convertible_v<_From, _To> &&
  requires (add_rvalue_reference_t<_From> (&__f)()) {
    static_cast<_To>(__f());
  };

template<class _Fn, class... _Args>
concept invocable = requires(_Fn&& __fn, _Args&&... __args) {
  _VSTD::invoke(_VSTD::forward<_Fn>(__fn), _VSTD::forward<_Args>(__args)...); // not required to be equality preserving
};
}
#endif

namespace atest
{
//! Concept that is `true` if `T` has method
//! `what()` that returns a value such that it can
//! be converted to `std::string`.
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

//! \brief The `ExpectToThrow<ExpressionT,
//! ExceptionT, ValueT, ExceptionValidationPolicy,
//! ExpectationType, FailurePolicy>` provides
//! exception related expectation.
//!
//! The type `ExpressionT` must be an
//! `std::invocable` type. Type `ExceptionT` is
//! the type of the exception. Type `ValueT` is
//! the value against which the exception of type
//! `ExceptionT` will be matched if
//! `ValidateValue` is set to `true`. If the
//! `ValidateValue` argument is set to `false` no
//! matching of the caught value beside `typeid`
//! matching will be performed.
//!
//! The type `ValueT` does not necessarily have to
//! be of the same type as `ExceptionT` but there
//! must be an `operator==` that takes both the
//! exception and `ValueT` values. Additionally if
//! the exception `ExceptionT` satisfies `HasWhat`
//! concept the value `ValueT` will be matched
//! against the `ExceptionT::what()` rather than
//! the exception object itself. For that
//! comparison the `ValueT` must either be
//! std::string or be convertible to std::string.
template<typename ExpressionT, typename ExceptionT, typename ValueT, ExceptionValidationPolicy validationPolicy, ExpectationType expectationType, FailurePolicy failurePolicy>
requires std::invocable<ExpressionT>
class ExpectToThrow : public ExpectBase<ExpressionT, expectationType, failurePolicy>
{
public:
    using ExpectBase<ExpressionT, expectationType, failurePolicy>::ExpectBase;

    //! Constructs the object taking the reference
    //! to `expression`, `value` and
    //! `sourceLocation`. Invokes `ExpressionT`
    //! expecting the exception of type
    //! `ExceptionT`. If an exception is thrown it
    //! will be validated against type and
    //! optionally against `value`.  Not throwing
    //! any exception, throwing unknown exception,
    //! throwin an exception of a different type
    //! or optionally with different value all
    //! fail the expectation.
    ExpectToThrow(const ExpressionT &expression, const ValueT &value, const std::source_location &sourceLocation) :
        ExpectBase<ValueT, expectationType, failurePolicy>{expression, sourceLocation},
        value{value}
    {
        try
        {
            this->expression()();
            this->handle_failure(Failure{"No exception thrown", ::atest::stringify(typeid(ExceptionT).name(), " '", this->value, '\''), ""});
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
    auto do_validate_exception_Value(const ExceptionT &exception) -> void
    {
        if (this->match_exception_value(exception))
        {
            this->handle_success();
        }
        else
        {
            this->handle_failure(Failure{"Exception value mismatch", ::atest::stringify(this->value), this->exception_to_string(exception)});
        }
    }

    [[nodiscard]] auto exception_to_string(const ExceptionT &exception) -> std::string
    {
        if constexpr (HasWhat<ExceptionT>)
        {
            return exception.what();
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
            this->handle_failure(Failure{"Exception type mismatch", typeid(ExceptionT).name(), typeid(exception).name()});
        }
        catch (...)
        {
            this->handle_failure(Failure{"Exception type mismatch", typeid(ExceptionT).name(), "Unknown exception"});
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

    auto validate_exception(ExceptionT &exception) -> void
    {
        if (typeid(ExceptionT) == typeid(exception))
        {
            this->validate_exception_value(exception);
        }
        else
        {
            this->handle_failure(Failure{"Exception type mismatch", typeid(ExceptionT).name(), typeid(exception).name()});
        }
    }

    auto validate_exception_value(ExceptionT &exception) -> void
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
