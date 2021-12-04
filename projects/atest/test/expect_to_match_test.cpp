import atest;

using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

class MyMatcher : public ::atest::MatcherBase
{
public:
    [[nodiscard]] static auto describe() -> std::string
    {
        return "Values were not matched";
    }

    [[nodiscard]] static auto expected(int left, int right) -> std::string
    {
        return std::to_string(left) + " < " + std::to_string(right);
    }

    [[nodiscard]] static auto actual(int left, int right) -> std::string
    {
        return std::to_string(left) + (left == right ? std::string{" == "} : std::string{" > "}) + std::to_string(right);
    }

    [[nodiscard]] auto operator()(int left, int right) -> bool
    {
        return left < right;
    }
};

static const auto S = suite("Expect::to_match()", [] { // NOLINT(cert-err58-cpp)
    test("Custom matcher", [] {
        expect(1).to_match<MyMatcher>(2);
        expect_fail(1).to_match<MyMatcher>(1);
        expect_fail(1).to_match<MyMatcher>(0);
    });

    test("Custom matcher (failing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect_fail(1).to_match<MyMatcher>(2);
                expect(1).to_match<MyMatcher>(1);
                expect(1).to_match<MyMatcher>(0);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Expected a failure but the expectation passed");
        expect(output.str()).to_contain("Values were not matched");
        expect(output.str()).to_contain("Expected:  1 < 1");
        expect(output.str()).to_contain("Actual  :  1 == 1");
        expect(output.str()).to_contain("Actual  :  1 > 0");
    });
});
