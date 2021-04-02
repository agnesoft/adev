import atest;

using atest::expect;
using atest::suite;
using atest::test;

static const auto s = suite("Expect::toThrow()", [] { //NOLINT(cert-err58-cpp)
    test("Exception type", [] {
        expect([] { throw std::logic_error{""}; }).template toThrow<std::logic_error>();
    });

    test("Wrong exception type", [] {
        expect([] { throw std::runtime_error{""}; }).template toThrow<std::logic_error>().toFail();
    });

    test("Base expected, derived thrown", [] {
        expect([] { throw std::runtime_error{""}; }).template toThrow<std::exception>().toFail();
    });

    test("Exception text", [] {
        expect([] { throw std::runtime_error{"Some exception text"}; }).template toThrow<std::runtime_error>("Some exception text");
    });

    test("Wrong exception text", [] {
        expect([] { throw std::runtime_error{"Some exception text"}; }).template toThrow<std::runtime_error>("Some different exception text").toFail();
    });

    test("No throw", [] {
        expect([] {}).template toThrow<std::exception>().toFail();
    });

    test("Throw int", [] {
        expect([] { throw 1; }).template toThrow<int>();
    });
});
