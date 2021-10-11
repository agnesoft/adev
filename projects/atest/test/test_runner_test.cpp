import atest;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("TestRunner", [] { // NOLINT(cert-err58-cpp)
    test("all tests pass", [] {
        std::stringstream output;
        int result = -1;

        {
            ::atest::TestRunner runner{output};

            test("test1", [] {
                expect(1).to_be(1);
            });

            test("test2", [] {
                expect(1).to_be(1);
            });

            result = runner.run(0, nullptr);
        }

        expect(result).to_be(0);
        expect(output.str()).to_contain("Result      : \033[1;32mPASSED\033[0m");
        expect(output.str()).to_contain("Tests       : 2 | 2 passed | 0 failed");
        expect(output.str()).to_contain("Expectations: 2 | 2 passed | 0 failed");
    });

    test("all tests pass but test throws", [] {
        std::stringstream output;
        int result = -1;

        {
            ::atest::TestRunner runner{output};

            test("test1", [] {
                expect(1).to_be(1);
            });

            test("test2", [] {
                expect(1).to_be(1);
            });

            test("test3", [] {
                expect(1).to_be(1);
                throw 1; // NOLINT(hicpp-exception-baseclass)
            });

            result = runner.run(0, nullptr);
        }

        expect(result).to_be(1);
        expect(output.str()).to_contain("Result      : \033[1;31mFAILED\033[0m");
        expect(output.str()).to_contain("Tests       : 3 | 2 passed | 1 failed");
        expect(output.str()).to_contain("Expectations: 3 | 3 passed | 0 failed");
    });

    test("1 failure", [] {
        std::stringstream output;
        int result = -1;

        {
            ::atest::TestRunner runner{output};

            test("test1", [] {
                expect(1).to_be(1);
            });

            test("test2", [] {
                expect(1).to_be(2);
            });

            result = runner.run(0, nullptr);
        }

        expect(result).to_be(1);
        expect(output.str()).to_contain("Result      : \033[1;31mFAILED\033[0m");
        expect(output.str()).to_contain("Tests       : 2 | 1 passed | 1 failed");
        expect(output.str()).to_contain("Expectations: 2 | 1 passed | 1 failed");
    });

    test("3 failures", [] {
        std::stringstream output;
        int result = -1;

        {
            ::atest::TestRunner runner{output};

            test("test1", [] {
                expect(1).to_be(1);
            });

            test("test2", [] {
                expect(1).to_be(2);
                throw std::exception{};
            });

            test("test3", [] {
                expect(1).to_be(3);
            });

            result = runner.run(0, nullptr);
        }

        expect(result).to_be(3);
        expect(output.str()).to_contain("Result      : \033[1;31mFAILED\033[0m");
        expect(output.str()).to_contain("Tests       : 3 | 1 passed | 2 failed");
        expect(output.str()).to_contain("Expectations: 3 | 1 passed | 2 failed");
    });
});
