import atest;

using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("Expect::toThrow()", [] { //NOLINT(cert-err58-cpp)
    test("Exception types match", [] {
        expect([] { throw std::logic_error{""}; }).to_throw<std::logic_error>();
    });

    test("Exception types mismatch", [] {
        expect_fail([] { throw std::runtime_error{""}; }).to_throw<std::logic_error>();
    });

    test("Exception types mismatch: expect base, throw derived", [] {
        expect_fail([] { throw std::runtime_error{""}; }).to_throw<std::exception>();
    });

    test("Exception types mismatch: throw int", [] {
        expect_fail([] { throw 1; }).to_throw<std::exception>();
    });

    test("Exception text match", [] {
        expect([] { throw std::runtime_error{"Some exception text"}; }).to_throw<std::runtime_error>("Some exception text");
    });

    test("Exception text match: int", [] {
        expect([] { throw std::logic_error{"1"}; }).to_throw<std::logic_error>(1);
    });

    test("Exception text mismatch", [] {
        expect_fail([] { throw std::runtime_error{"Some exception text"}; }).to_throw<std::runtime_error>("Some different exception text");
    });

    test("Exception text mismatch (INTENTIONAL FAILURE)", [] {
        expect([] { throw std::string{"error"}; }).to_throw<std::string>("different error");
    });

    test("No exception thrown", [] {
        expect_fail([] {}).to_throw<std::exception>();
    });

    test("No exception thrown (INTENTIONAL FAILURE)", [] {
        expect([] {}).to_throw<std::exception>();
    });

    test("Exception types match: int", [] {
        expect([] { throw 1; }).to_throw<int>();
    });

    test("Exception value match: int", [] {
        expect([] { throw 1; }).to_throw(1);
    });

    test("Exception value match: std::string", [] {
        expect([] { throw std::string{"Exception text"}; }).to_throw(std::string{"Exception text"});
    });

    test("Exception value match: const char*", [] {
        expect([] { throw std::string{"Exception text"}; }).to_throw<std::string>("Exception text");
    });
});
