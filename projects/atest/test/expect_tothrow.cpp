import atest;

using atest::expect;
using atest::suite;
using atest::test;

static auto s = suite("Expect::toThrow()", [] {
    test("Exception type", [] {
        expect([] { throw std::logic_error{""}; }).toThrow<std::logic_error>("");
    });

    test("Wrong exception type", [] {
        expect([] { throw std::runtime_error{""}; }).toThrow<std::logic_error>("").toFail();
    });

    test("Base exception expected but derived thrown", [] {
        expect([] { throw std::runtime_error{""}; }).toThrow<std::exception>("").toFail();
    });

    test("Exception text", [] {
        expect([] { throw std::runtime_error{"Some exception text"}; }).toThrow<std::runtime_error>("Some exception text");
    });

    test("Wrong exception text", [] {
        expect([] { throw std::runtime_error{"Some exception text"}; }).toThrow<std::runtime_error>("Some different exception text").toFail();
    });

    test("No throw", [] {
        expect([] {}).toThrow<std::exception>("").toFail();
    });
});
