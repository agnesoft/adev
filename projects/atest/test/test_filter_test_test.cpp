import atest;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("test filter (test)", [] { // NOLINT(cert-err58-cpp)
    test("run 1 test", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test1", [] {
                expect(1).to_be(1);
            });

            test("test2", [] {
                expect(1).to_be(1);
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "--test=test1"}.data()));
        }

        expect(output.str()).to_contain("Running 1 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 1 | 1 passed | 0 failed");
        expect(output.str()).to_contain("test1");
        expect(output.str()).not_to_contain("test2");
    });

    test("run 2 tests", [] {
        std::stringstream output;

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
            });

            static_cast<void>(runner.run(4, std::array<const char *, 4>{"./app_test", "--test=test1", "-t", "test2"}.data()));
        }

        expect(output.str()).to_contain("Running 2 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 2 | 2 passed | 0 failed");
        expect(output.str()).to_contain("test1");
        expect(output.str()).to_contain("test2");
        expect(output.str()).not_to_contain("test3");
    });

    test("run tests with wildcard at end", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test11", [] {
                expect(1).to_be(1);
            });

            test("test235 aa", [] {
                expect(1).to_be(1);
            });

            test("my test", [] {
                expect(1).to_be(1);
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "--test=test*"}.data()));
        }

        expect(output.str()).to_contain("Running 2 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 2 | 2 passed | 0 failed");
        expect(output.str()).to_contain("test11");
        expect(output.str()).to_contain("test235 aa");
        expect(output.str()).not_to_contain("my test");
    });

    test("run tests with wildcard in front", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("yay test", [] {
                expect(1).to_be(1);
            });

            test("lol test", [] {
                expect(1).to_be(1);
            });

            test("my tst", [] {
                expect(1).to_be(1);
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "--test=*test"}.data()));
        }

        expect(output.str()).to_contain("Running 2 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 2 | 2 passed | 0 failed");
        expect(output.str()).to_contain("yay test");
        expect(output.str()).to_contain("lol test");
        expect(output.str()).not_to_contain("my tst");
    });

    test("run tests with wildcards in front and back", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("yay testing", [] {
                expect(1).to_be(1);
            });

            test("lol test me", [] {
                expect(1).to_be(1);
            });

            test("my tst not me", [] {
                expect(1).to_be(1);
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "--test=*test*"}.data()));
        }

        expect(output.str()).to_contain("Running 2 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 2 | 2 passed | 0 failed");
        expect(output.str()).to_contain("yay testing");
        expect(output.str()).to_contain("lol test me");
        expect(output.str()).not_to_contain("my tst not me");
    });

    test("empty name", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("", [] {
                expect(1).to_be(1);
            });

            static_cast<void>(runner.run(1, nullptr));
        }

        expect(output.str()).to_contain("Running 1 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 1 | 1 passed | 0 failed");
    });

    test("empty filter", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("", [] {
                expect(1).to_be(1);
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "--test=\"\""}.data()));
        }

        expect(output.str()).to_contain("Running 1 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 1 | 1 passed | 0 failed");
    });
});
