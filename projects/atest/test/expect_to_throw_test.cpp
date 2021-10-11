import atest;

using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Expect::to_throw()", [] { // NOLINT(cert-err58-cpp)
    test("types match", [] {
        expect([] { throw std::logic_error{""}; }).to_throw<std::logic_error>();
    });

    test("types mismatch", [] {
        expect_fail([] { throw std::runtime_error{""}; }).to_throw<std::logic_error>();
    });

    test("types mismatch: expect base, throw derived", [] {
        expect_fail([] { throw std::runtime_error{""}; }).to_throw<std::exception>();
    });

    test("types mismatch: throw int", [] {
        expect_fail([] { throw 1; }).to_throw<std::exception>(); // NOLINT(hicpp-exception-baseclass)
    });

    test("text match", [] {
        expect([] { throw std::runtime_error{"Some exception text"}; }).to_throw<std::runtime_error>("Some exception text");
    });

    test("text match: int", [] {
        expect([] { throw std::logic_error{"1"}; }).to_throw<std::logic_error>(1);
    });

    test("text mismatch", [] {
        expect_fail([] { throw std::runtime_error{"Some exception text"}; }).to_throw<std::runtime_error>("Some different exception text");
    });

    test("nothing thrown", [] {
        expect_fail([] {}).to_throw<std::exception>();
    });

    test("types match: int", [] {
        expect([] { throw 1; }).to_throw<int>(); // NOLINT(hicpp-exception-baseclass)
    });

    test("value match: int", [] {
        expect([] { throw 1; }).to_throw(1); // NOLINT(hicpp-exception-baseclass)
    });

    test("value match: std::string", [] {
        expect([] { throw std::string{"Exception text"}; }).to_throw(std::string{"Exception text"}); // NOLINT(hicpp-exception-baseclass)
    });

    test("value match: const char*", [] {
        expect([] { throw std::string{"Exception text"}; }).to_throw<std::string>("Exception text"); // NOLINT(hicpp-exception-baseclass)
    });

    test("text mismatch (failing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect([] { throw std::string{"error"}; }).to_throw<std::string>("different error"); // NOLINT(hicpp-exception-baseclass)
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Exception value mismatch");
        expect(output.str()).to_contain("Expected:  different error");
        expect(output.str()).to_contain("Actual  :  error");
    });

    test("no exception thrown (failing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect([] {}).to_throw<std::exception>();
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("No exception thrown");
        expect(output.str()).to_contain("Expected:  " + std::string{typeid(std::exception).name()} + " ''");
    });
});
