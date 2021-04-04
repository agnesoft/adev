import atest;

using atest::expect;
using atest::suite;
using atest::test;

static const auto s = suite("Expect::toThrow()", [] { //NOLINT(cert-err58-cpp)
    test("Expected type match", [] {
        expect([] { throw std::logic_error{""}; }).template toThrow<std::logic_error>();
    });

    test("Expected type mismatch", [] {
        expect([] { throw std::runtime_error{""}; }).template toThrow<std::logic_error>().toFail();
    });

    test("Expected type mismatch (derived)", [] {
        expect([] { throw std::runtime_error{""}; }).template toThrow<std::exception>().toFail();
    });

    test("Exception text", [] {
        expect([] { throw std::runtime_error{"Some exception text"}; }).template toThrow<std::runtime_error>("Some exception text");
    });

    test("Exception text mismatch", [] {
        expect([] { throw std::runtime_error{"Some exception text"}; }).template toThrow<std::runtime_error>("Some different exception text").toFail();
    });

    test("No throw", [] {
        expect([] {}).template toThrow<std::exception>().toFail();
    });

    test("Throw int", [] {
        expect([] { throw 1; }).template toThrow<int>();
    });

    test("Throw int, expect value", [] {
        expect([] { throw 1; }).toThrow(1);
    });

    test("Throw std::string", [] {
        expect([] { throw std::string{"Exception text"}; }).toThrow(std::string{"Exception text"});
    });

    test("Throw std::string, expect const char*", [] {
        expect([] { throw std::string{"Exception text"}; }).toThrow<std::string>("Exception text");
    });

    test("Throw text, expect int", [] {
        expect([] { throw std::logic_error{"1"}; }).toThrow<std::logic_error>(1);
    });
});
