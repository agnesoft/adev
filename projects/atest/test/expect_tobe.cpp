import atest;

using namespace atest;

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
        const auto callable = [] {
            return 1 + 1;
        };

        expect(callable).toBe(2);
    });

    test("callable throws", [] {
        const auto badCallable = []() -> int {
            throw std::runtime_error{"bad call"};
        };

        expect(badCallable).toBe(1);
    });
});
