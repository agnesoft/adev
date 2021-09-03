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
enum class FailurePolicy
{
    PassOnFailure,
    FailOnFailure
};

//! \brief The `ExpectBase<ExpressionT, ExpectationType,
//! FailurePolicy>` is the base class for all
//! expecations.
//!
//! The class provides basic functionality such as
//! source location, holding the `ExpressionT`,
//! error handling, result reversal and assertions.
template<typename ExpressionT, ExpectationType expectationType, FailurePolicy failurePolicy>
class ExpectBase
{
public:
    //! Constructs the object.
    explicit ExpectBase(const ExpressionT &expression, const std::source_location &sourceLocation) noexcept :
        expressionT{expression},
        sourceLocation{sourceLocation}
    {
        ::atest::global_tests().currentTest->expectations++;
    }

protected:
    //! Returns the expression.
    [[nodiscard]] auto expression() const noexcept -> const ExpressionT &
    {
        return this->expressionT;
    }

    //! Fails the expectation. If `ExpectFail` is
    //! `true` the failure will be converted to a
    //! success.
    auto handle_failure(Failure &&failure) -> void
    {
        if constexpr (failurePolicy == FailurePolicy::FailOnFailure)
        {
            this->fail(std::move(failure));
        }
    }

    //! Passes the expectation. If `ExpectFail` is
    //! `true` the failure will be converted to a
    //! failure.
    auto handle_success() -> void
    {
        if constexpr (failurePolicy == FailurePolicy::PassOnFailure)
        {
            this->fail(Failure{"Expected a failure but the expectation passed."});
        }
    }

    //! Fails the expectation and records the
    //! failure to the currently running test.
    //! Only call this method if you want to fail
    //! the expectation regardless of `ExpectFail`
    //! value.
    auto fail(Failure &&failure) -> void
    {
        failure.sourceLocation = this->sourceLocation;
        ::atest::global_tests().currentTest->failedExpectations++;
        ::atest::global_tests().currentTest->failures.emplace_back(std::move(failure));

        if constexpr (expectationType == ExpectationType::Assertion)
        {
            throw FailedAssertion{};
        }
    }

private:
    const ExpressionT &expressionT;
    std::source_location sourceLocation;
};
}
