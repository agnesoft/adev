import atest;

using atest::expect;
using atest::suite;
using atest::test;

class MyMatcher : public atest::MatcherBase
{
public:
    [[nodiscard]] auto describe() const -> std::string
    {
        return "Left value to be less than right value.";
    }

    [[nodiscard]] auto expected(int left, int right) const -> std::string
    {
        return std::to_string(left) + " < " + std::to_string(right);
    }

    [[nodiscard]] auto actual(int left, int right) const -> std::string
    {
        return std::to_string(left) + (left == right ? " = " : " > ") + std::to_string(right);
    }

    [[nodiscard]] auto operator()(int left, int right) -> bool
    {
        return left < right;
    }
};

static auto s = suite("Expect::toMatch()", [] {
    test("Custom comparison", [] {
        expect(1).template toMatch<MyMatcher>(1).toFail();
    });
});
