import atest;

using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("Expect::toBe()", [] { //NOLINT(cert-err58-cpp)
    test("Value is equal to itself", [] {
        expect(1).to_be(1);
    });

    test("Expect failure but pass (INTENTIONAL FAILURE)", [] {
        expect_fail(1).to_be(1);
    });

    test("Callable is equal to value", [] {
        expect([] { return 1 + 1; }).to_be(2);
    });

    test("Value is not equal a different value (INTENTIONAL FAILURE)", [] {
        expect(1).to_be(2);
    });

    test("Value is not equal a different value", [] {
        expect_fail(1).to_be(2);
    });

    test("Match array", [] {
        expect(std::string{}).to_be("");
    });

    test("Callable is expected to fail but throws an exception (INTENTIONAL FAILURE)", [] {
        expect_fail([]() -> int { throw std::runtime_error{"bad call"}; }).to_be(1);
    });

    test("Callable is expected to fail but throws an int (INTENTIONAL FAILURE)", [] {
        expect_fail([]() -> int { throw 1; }).to_be(1); //NOLINT(hicpp-exception-baseclass)
    });
});
