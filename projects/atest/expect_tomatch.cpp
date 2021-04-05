module;

#ifdef _MSC_VER
#    include <typeinfo>
#endif

export module atest : expect_tomatch;

export import : expect_base;

namespace atest
{
export template<typename T, typename V, typename Matcher>
class ExpectToMatch : public ExpectBase<T>
{
public:
    ExpectToMatch(const T &expression, const V &value, const source_location<> &sourceLocation) :
        ExpectBase<T>{expression, sourceLocation},
        mValue{value}
    {
    }

    ExpectToMatch(const ExpectToMatch &other) = default;
    ExpectToMatch(ExpectToMatch &&other) noexcept = default;

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
