import atest;

using atest::expect;
using atest::suite;
using atest::test;

class MyMatcher : public atest::MatcherBase
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

static const auto s = suite("Expect::toMatch()", [] { //NOLINT(cert-err58-cpp)
    test("Custom matcher", [] {
        expect(1).template toMatch<MyMatcher>(2);
        expect(1).template toMatch<MyMatcher>(1).toFail();
        expect(1).template toMatch<MyMatcher>(0).toFail();
    });

    test("Custom matcher (INTENTIONAL FAILURE)", [] {
        expect(1).template toMatch<MyMatcher>(2).toFail();
        expect(1).template toMatch<MyMatcher>(1);
        expect(1).template toMatch<MyMatcher>(0);
    });
});
