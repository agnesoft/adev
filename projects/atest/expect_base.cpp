#ifdef _MSC_VER
export module atest : expect_base;

import : data;
#endif

namespace atest
{
//! \brief The `ExpectBase<T>` is the base class for all expecations.
//!
//! The class provides basic functionality such as source_location
//! and holds the expression `T`. The `T` can be any type (value or callable).
//! The class also provides success and error handling for expectations and
//! implements one public interface function toFail() to request reversal of
//! the expectation's result.
export template<typename T>
class ExpectBase
{
public:
    //! Constructs the object taking the `expression` and `sourceLocation`.
    explicit ExpectBase(const T &expression, const source_location<> &sourceLocation) noexcept :
        mExpression{expression},
        mSourceLocation{sourceLocation}
    {
        globalTests()->currentTest->expectations++;
    }

    //! Reverses the result of the expectation. If the handleSuceed() is called
    //! the expectation will actually fail. If conversely the handleFailed()
    //! is called the expectation will pass.
    auto toFail() noexcept -> void
    {
        mExpectFailure = true;
    }

protected:
    //! Returns the `const T &` expression.
    [[nodiscard]] auto expression() const noexcept -> const T &
    {
        return mExpression;
    }

    //! Failes the expectation. This method should be called by the derived
    //! classes rather than fail() directly as it allows the reversal of the
    //! reesult to take place if toFail() was called.
    auto handleFailure(Failure &&failure) -> void
    {
        if (!mExpectFailure)
        {
            fail(std::move(failure));
        }
    }

    //! Passes the expectation. This method should be called by the derived
    //! classes rather than fail() directly as it allows the reversal of the
    //! reesult to take place if toFail() was called.
    auto handleSuccess() -> void
    {
        if (mExpectFailure)
        {
            fail(Failure{"Expected a failure but the test succeeded."});
        }
    }

    //! Failes the expectation and records the failure to the currently
    //! running test. Only call this method if you want to fail the expectation
    //! and disregard any reversal requested by the call to toFail().
    auto fail(Failure &&failure) -> void
    {
        failure.sourceLocation = mSourceLocation;
        globalTests()->currentTest->failedExpectations++;
        globalTests()->currentTest->failures.emplace_back(std::move(failure));
    }

private:
    const T &mExpression;
    bool mExpectFailure = false;
    source_location<> mSourceLocation;
};
}
