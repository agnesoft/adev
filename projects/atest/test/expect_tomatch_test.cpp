import atest;

using atest::expect;
using atest::suite;
using atest::test;

class MyMatcher : public atest::MatcherBase
{
public:
    [[nodiscard]] static auto describe() -> std::string
    {
        return "Left value to be less than right value.";
    }

    [[nodiscard]] static auto expected(int left, int right) -> std::string
    {
        return std::to_string(left) + " < " + std::to_string(right);
    }

    [[nodiscard]] static auto actual(int left, int right) -> std::string
    {
        return std::to_string(left) + (left == right ? std::string{" = "} : std::string{" > "}) + std::to_string(right);
    }

    [[nodiscard]] auto operator()(int left, int right) -> bool
    {
        return left < right;
    }
};

static const auto s = suite("Expect::toMatch()", [] { //NOLINT(cert-err58-cpp)
    test("Custom comparison", [] {
        expect(1).template toMatch<MyMatcher>(1).toFail();
    });
});
