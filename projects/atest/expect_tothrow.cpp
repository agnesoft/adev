#ifdef _MSC_VER
module;

#    include <typeinfo>

export module atest : expect_tothrow;

export import : expect_base;
#endif

namespace atest
{
//! Concept that is `true` if `T` has method
//! what() that returns a value such that it can
//! be converted to `std::string`.
export template<typename T>
concept hasWhat = requires(const T &type)
{
    // clang-format off
    {type.what()} -> std::convertible_to<std::string>;
    // clang-format on
};

//! \brief The `ExpectToThrow<T, E, V, ValidateValue>` provides exception
//! related assertions.
//!
//! The type `T` must be an std::invocable type. Type `E` is the type of the
//! exception. Type `V` is the value against which the exception of type `E`
//! will be matched if `ValidateValue` is set to `true`. If the `ValidateValue`
//! argument is set to `false` no matching of the caught value beside `typeid`
//! matching will be performed.
//!
//! The type `V` does not neccesarily have to be of the same type as `E` but
//! there must be an `operator==` that takes both the exception and `V` value.
//! Additionally if the exception `E` satisfies hasWhat concept the value
//! `V` will be matched against the `E::what()` rather than the exception object
//! itself. For that comparison the `V` must either be or be convertible to std::string.
export template<typename T, typename E, typename V, bool ValidateValue, bool Assert, bool ExpectFail>
requires std::invocable<T>
class ExpectToThrow : public ExpectBase<T, Assert, ExpectFail>
{
public:
    using ExpectBase<T, Assert, ExpectFail>::ExpectBase;

    //! Constructs the object taking the reference to `expression`, `value` and
    //! source_location and runs `T` expecting the exception of type
    //! `E` and validating its type and optionally its value. No
    //! exception is propagated outside. Not throwing any exception
    //! or throwing unknown exception, exception of a different than
    //! expected type or not matching the value `V` (if value matching
    //! is requested) all result in an error and failure of the expectation.
    ExpectToThrow(const T &expression, const V &value, const source_location<> &sourceLocation) :
        ExpectBase<T, Assert, ExpectFail>{expression, sourceLocation},
        mValue{value}
    {
#ifdef _MSC_VER
        using ::type_info;
#endif
        try
        {
            this->expression()();
            this->handleFailure(Failure{"No exception thrown", stringify(typeid(E).name(), " '", mValue, '\''), ""});
        }
        catch ([[maybe_unused]] FailedAssertion &e)
        {
            throw;
        }
        catch (E &e)
        {
            validateException(e);
        }
        catch (...)
        {
            handleUnknownException();
        }
    }

private:
    auto doValidateExceptionValue(const E &e) -> void
    {
        if (matchExceptionValue(e))
        {
            this->handleSuccess();
        }
        else
        {
            this->handleFailure(Failure{"Exception value mismatch", stringify(mValue), exceptionToString(e)});
        }
    }

    [[nodiscard]] auto exceptionToString(const E &e) -> std::string
    {
        if constexpr (hasWhat<E>)
        {
            return e.what();
        }
        else
        {
            return stringify(e);
        }
    }

    auto handleUnknownException()
    {
#ifdef _MSC_VER
        using ::type_info;
#endif

        try
        {
            throw;
        }
        catch (std::exception &e)
        {
            this->handleFailure(Failure{"Exception type mismatch", typeid(E).name(), typeid(e).name()});
        }
        catch (...)
        {
            this->handleFailure(Failure{"Exception type mismatch", typeid(E).name(), "Unknown exception"});
        }
    }

    [[nodiscard]] auto matchExceptionValue(const E &e) -> bool
    {
        if constexpr (hasWhat<E>)
        {
            return stringify(mValue) == e.what();
        }
        else
        {
            return mValue == e;
        }
    }

    auto validateException(E &e) -> void
    {
#ifdef _MSC_VER
        using ::type_info;
#endif

        if (typeid(E) == typeid(e))
        {
            validateExceptionValue(e);
        }
        else
        {
            this->handleFailure(Failure{"Exception type mismatch", typeid(E).name(), typeid(e).name()});
        }
    }

    auto validateExceptionValue(E &e) -> void
    {
        if constexpr (ValidateValue)
        {
            doValidateExceptionValue(e);
        }
        else
        {
            this->handleSuccess();
        }
    }

    const V &mValue;
};
}
