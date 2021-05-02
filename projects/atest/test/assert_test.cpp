import atest;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto s = suite("assert()", [] { //NOLINT(cert-err58-cpp)
    test("assert stops the test (INTENTIONAL FAILURE)", [] {
        assert_(1).toBe(2);
        expect(1).toBe(2);
    });

    test("assert stops the test throwing (INTENTIONAL FAILURE)", [] {
        assert_([] {}).template toThrow<int>();
        expect(1).toBe(2);
    });

    test("assert_fail().toBe()", [] {
        assert_fail(1).toBe(2);
    });

    test("assert_fail().toThrow()", [] {
        assert_fail([] {}).template toThrow<int>();
    });
});
