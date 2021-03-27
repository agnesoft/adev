import atest;

using namespace atest;

static auto s = suite("Expect::toThrow()", [] {
    test("Exception type", [] {
        const auto throwingCallable = [] {
            throw std::logic_error{""};
        };

        expect(throwingCallable).toThrow<std::logic_error>("");
    });

    test("Wrong exception type", [] {
        const auto throwingCallable = [] {
            throw std::runtime_error{""};
        };

        expect(throwingCallable).toThrow<std::logic_error>("");
    });

    test("Exception text", [] {
        const auto throwingCallable = [] {
            throw std::runtime_error{"Some exception text"};
        };

        expect(throwingCallable).toThrow<std::runtime_error>("Some exception text");
    });

    test("Wrong exception text", [] {
        const auto throwingCallable = [] {
            throw std::runtime_error{"Some exception text"};
        };

        expect(throwingCallable).toThrow<std::runtime_error>("Some different exception text");
    });

    test("No throw", [] {
        const auto notThrowingCallable = [] {
        };

        expect(notThrowingCallable).toThrow<std::exception>("");
    });
});