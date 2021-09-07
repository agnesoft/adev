import atest;

using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("assert()", [] { //NOLINT(cert-err58-cpp)
    test("assert stops the test (INTENTIONAL FAILURE)", [] {
        assert_(1).to_be(2);
        expect(1).to_be(2);
    });

    test("assert stops the test throwing (INTENTIONAL FAILURE)", [] {
        assert_([] {}).template to_throw<int>();
        expect(1).to_be(2);
    });

    test("assert_fail().toBe()", [] {
        assert_fail(1).to_be(2);
    });

    test("assert_fail().toThrow()", [] {
        assert_fail([] {}).template to_throw<int>();
    });
});
