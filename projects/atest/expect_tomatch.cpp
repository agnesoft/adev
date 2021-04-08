#ifdef _MSC_VER
module;

#    include <typeinfo>

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
export template<typename T, typename V, typename Matcher>
class ExpectToMatch : public ExpectBase<T>
{
public:
    //! Constructs the object with the `expression`, `value` and `sourceLocation`.
    ExpectToMatch(const T &expression, const V &value, const source_location<> &sourceLocation) :
        ExpectBase<T>{expression, sourceLocation},
        mValue{value}
    {
    }

    //! Defaulted copy constructor.
    ExpectToMatch(const ExpectToMatch &other) = default;

    //! Defaulted move constructor.
    ExpectToMatch(ExpectToMatch &&other) noexcept = default;

    //! Destructor that actually runs the matching. If `T` is a callable type
    //! it will first be called and only its result will be compared against value
    //! `V`. No exceptions are propagated from such call. The matching is done by
    //! calling the `operator()` of the `Matcher`. If the match fails `Matcher`
    //! will be queried for the details regarding the match and provided with
    //! the values to possibly stringify for the error output.
    ~ExpectToMatch()
    {
#ifdef _MSC_VER
        using ::type_info;
#endif

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
        catch (std::exception &e)
        {
            this->fail(Failure{stringify("Unexpected exception thrown (", typeid(e).name(), "): ", e.what())});
        }
        catch (...)
        {
            this->fail(Failure{"Unexpected exception thrown"});
        }
    }

    auto operator=(const ExpectToMatch &other) -> ExpectToMatch & = default;
    auto operator=(ExpectToMatch &&other) noexcept -> ExpectToMatch & = default;

private:
    [[nodiscard]] auto evaluateExpression() const -> V
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
