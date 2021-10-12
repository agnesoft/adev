import atest;

using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("test filter", [] { // NOLINT(cert-err58-cpp)
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

        expect(output.str()).to_contain("test1");
        expect_fail(output.str()).to_contain("test2");
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

        expect(output.str()).to_contain("test1");
        expect(output.str()).to_contain("test2");
        expect_fail(output.str()).to_contain("test3");
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

        expect(output.str()).to_contain("test11");
        expect(output.str()).to_contain("test235 aa");
        expect_fail(output.str()).to_contain("my test");
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

        expect(output.str()).to_contain("yay test");
        expect(output.str()).to_contain("lol test");
        expect_fail(output.str()).to_contain("my tst");
    });
});
