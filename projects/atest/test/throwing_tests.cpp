import atest;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("throwing tests", [] { // NOLINT(cert-err58-cpp)
    test("test() throws int", [] {
        expect([] {
            std::stringstream output;
            ::atest::TestRunner runner{output};

            test("Bad test", [] {
                throw 1; // NOLINT(hicpp-exception-baseclass)
            });

            return runner.run(0, nullptr);
        })
            .to_be(1);
    });

    test("test() throws int (text)", [] {
        const std::string output = [] {
            std::stringstream output;
            ::atest::TestRunner runner{output};

            test("Bad test", [] {
                throw 1; // NOLINT(hicpp-exception-baseclass)
            });

            static_cast<void>(runner.run(0, nullptr));
            return output.str();
        }();

        expect(output).to_contain("Unexpected exception thrown");
    });
});
