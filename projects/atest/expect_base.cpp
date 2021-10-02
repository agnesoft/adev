#ifndef __clang__
module atest : expect_base;
import : failed_assertion;
import : global_tests;
#endif

namespace atest
{
//! \private
enum class ExpectationType
{
    Assertion,
    Expectation
};

//! \private
enum class ResultHandlingPolicy
{
    Normal,
    Reverse
};

//! \private
template<typename ExpressionT, ExpectationType expectationType, ResultHandlingPolicy resultHandlingPolicy>
class ExpectBase
{
public:
    explicit ExpectBase(const ExpressionT &expression, const std::source_location &sourceLocation) noexcept :
        expressionT{expression},
        sourceLocation{sourceLocation}
    {
        ::atest::global_tests().currentTest->expectations++;
    }

protected:
    //! \private
    [[nodiscard]] auto expression() const noexcept -> const ExpressionT &
    {
        return this->expressionT;
    }

    //! \private
    auto handle_failure(Failure &&failure) -> void
    {
        if constexpr (resultHandlingPolicy == ResultHandlingPolicy::Normal)
        {
            this->fail(std::move(failure));
        }
    }

    //! \private
    auto handle_success() -> void
    {
        if constexpr (resultHandlingPolicy == ResultHandlingPolicy::Reverse)
        {
            this->fail(Failure{"Expected a failure but the expectation passed."});
        }
    }

    //! \private
    //!
    //! Only call this method if you want to fail
    //! the expectation and bypass the
    //! `resultHandlingPolicy` value.
    auto fail(Failure &&failure) -> void
    {
        failure.sourceLocation = this->sourceLocation;
        ::atest::global_tests().currentTest->failedExpectations++;
        ::atest::global_tests().currentTest->failures.emplace_back(std::move(failure));

        if constexpr (expectationType == ExpectationType::Assertion)
        {
            throw FailedAssertion{}; // NOLINT(hicpp-exception-baseclass)
        }
    }

private:
    const ExpressionT &expressionT;
    std::source_location sourceLocation;
};
}
