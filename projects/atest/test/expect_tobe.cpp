import atest;

using atest::expect;
using atest::suite;
using atest::test;

static auto s = suite("Expect::toBe()", [] {
    test("value == value", [] {
        expect(1).toBe(1);
    });

    test("value != value", [] {
        expect(1).toBe(2);
    });

    test("call == value", [] {
        const auto sum = [](int x, int y) {
            return x + y;
        };

        expect(sum(1, 1)).toBe(2);
    });

    test("callable == value", [] {
        expect([] { return 1 + 1; }).toBe(2);
    });

    test("callable throws", [] {
        expect([]() -> int { throw std::runtime_error{"bad call"}; }).toBe(1);
    });
});
