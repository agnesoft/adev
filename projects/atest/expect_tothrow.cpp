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

export template<typename T, typename E, typename V, bool ValidateValue>
requires std::invocable<T> class ExpectToThrow : public ExpectBase<T>
{
public:
    using ExpectBase<T>::ExpectBase;

    ExpectToThrow(const T &expression, const V &value, const source_location<> &sourceLocation) :
        ExpectBase<T>{expression, sourceLocation},
        mValue{value}
    {
    }

    ExpectToThrow(const ExpectToThrow &other) = default;
    ExpectToThrow(ExpectToThrow &&other) noexcept = default;

    ~ExpectToThrow()
    {
#ifdef _MSC_VER
        using ::type_info;
#endif

        try
        {
            this->expression()();
            this->handleFailure(Failure{"No exception thrown", stringify(typeid(E).name(), " '", mValue, '\''), ""});
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

    auto operator=(const ExpectToThrow &other) -> ExpectToThrow & = default;
    auto operator=(ExpectToThrow &&other) noexcept -> ExpectToThrow & = default;

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
