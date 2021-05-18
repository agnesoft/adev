import atest;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto s = suite("Expect::toThrow()", [] { //NOLINT(cert-err58-cpp)
    test("Exception types match", [] {
        expect([] { throw std::logic_error{""}; }).toThrow<std::logic_error>();
    });

    test("Exception types mismatch", [] {
        expect_fail([] { throw std::runtime_error{""}; }).toThrow<std::logic_error>();
    });

    test("Exception types mismatch: expect base, throw derived", [] {
        expect_fail([] { throw std::runtime_error{""}; }).toThrow<std::exception>();
    });

    test("Exception types mismatch: throw int", [] {
        expect_fail([] { throw 1; }).toThrow<std::exception>();
    });

    test("Exception text match", [] {
        expect([] { throw std::runtime_error{"Some exception text"}; }).toThrow<std::runtime_error>("Some exception text");
    });

    test("Exception text match: int", [] {
        expect([] { throw std::logic_error{"1"}; }).toThrow<std::logic_error>(1);
    });

    test("Exception text mismatch", [] {
        expect_fail([] { throw std::runtime_error{"Some exception text"}; }).toThrow<std::runtime_error>("Some different exception text");
    });

    test("Exception text mismatch (INTENTIONAL FAILURE)", [] {
        expect([] { throw std::string{"error"}; }).toThrow<std::string>("different error");
    });

    test("No exception thrown", [] {
        expect_fail([] {}).toThrow<std::exception>();
    });

    test("No exception thrown (INTENTIONAL FAILURE)", [] {
        expect([] {}).toThrow<std::exception>();
    });

    test("Exception types match: int", [] {
        expect([] { throw 1; }).toThrow<int>();
    });

    test("Exception value match: int", [] {
        expect([] { throw 1; }).toThrow(1);
    });

    test("Exception value match: std::string", [] {
        expect([] { throw std::string{"Exception text"}; }).toThrow(std::string{"Exception text"});
    });

    test("Exception value match: const char*", [] {
        expect([] { throw std::string{"Exception text"}; }).toThrow<std::string>("Exception text");
    });
});
