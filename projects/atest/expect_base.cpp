export module atest : expect_base;

import : data;

namespace atest
{
export template<typename T>
class ExpectBase
{
public:
    explicit ExpectBase(const T &expression, const source_location<> &sourceLocation) noexcept :
        mExpression{expression},
        mSourceLocation{sourceLocation}
    {
        globalTests()->currentTest->expectations++;
    }

    auto toFail() noexcept -> void
    {
        mExpectFailure = true;
    }

protected:
    [[nodiscard]] auto expression() const noexcept -> const T &
    {
        return mExpression;
    }

    auto handleFailure(Failure &&failure) -> void
    {
        if (!mExpectFailure)
        {
            fail(std::move(failure));
        }
    }

    auto handleSuccess() -> void
    {
        if (mExpectFailure)
        {
            fail(Failure{"Expected a failure but the test succeeded."});
        }
    }

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
