import atest;

using atest::expect;
using atest::suite;
using atest::test;

static const auto s = suite("Expect::toBe()", [] { //NOLINT(cert-err58-cpp)
    test("value == value", [] {
        expect(1).toBe(1);
    });

    test("call == value", [] {
        const auto sum = [](int x, int y) noexcept {
            return x + y;
        };

        expect(sum(1, 1)).toBe(2);
    });

    test("callable == value", [] {
        expect([] { return 1 + 1; }).toBe(2);
    });

    test("value != value", [] {
        expect(1).toBe(2).toFail();
    });

    test("callable throws std::exception", [] {
        expect([]() -> int { throw std::runtime_error{"bad call"}; }).toBe(1).toFail();
    });

    test("callable throws int", [] {
        expect([]() -> int { throw 1; }).toBe(1).toFail();
    });
});
