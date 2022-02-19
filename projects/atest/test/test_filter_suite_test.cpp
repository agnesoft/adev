import atest;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("test filter (suite)", [] { // NOLINT(cert-err58-cpp)
    test("run 1 suite", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            suite("my suite", [] {
                test("yay testing", [] {
                    expect(1).to_be(1);
                });

                test("lol test me", [] {
                    expect(1).to_be(1);
                });
            });

            suite("other suite to run", [] {
                test("my tst", [] {
                    expect(1).to_be(1);
                });
            });

            suite("do not run", [] {
                test("do not run this either", [] {
                    expect(1).to_be(1);
                });
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "-s=\"other suite to run\""}.data()));
        }

        expect(output.str()).to_contain("Running 1 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 1 | 1 passed | 0 failed");
        expect(output.str()).not_to_contain("my suite");
        expect(output.str()).not_to_contain("yay testing");
        expect(output.str()).not_to_contain("lol test me");
        expect(output.str()).to_contain("other suite to run");
        expect(output.str()).to_contain("my tst");
        expect(output.str()).not_to_contain("do not run");
        expect(output.str()).not_to_contain("do not run this either");
    });

    test("run 2 suites", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            suite("my suite", [] {
                test("yay testing", [] {
                    expect(1).to_be(1);
                });

                test("lol test me", [] {
                    expect(1).to_be(1);
                });
            });

            suite("other suite to run", [] {
                test("my tst", [] {
                    expect(1).to_be(1);
                });
            });

            suite("do not run", [] {
                test("do not run this either", [] {
                    expect(1).to_be(1);
                });
            });

            static_cast<void>(runner.run(3, std::array<const char *, 3>{"./app_test", "--suite=\"my suite\"", "-s=\"other suite to run\""}.data()));
        }

        expect(output.str()).to_contain("Running 3 tests from 2 test suites...");
        expect(output.str()).to_contain("Tests       : 3 | 3 passed | 0 failed");
        expect(output.str()).to_contain("my suite");
        expect(output.str()).to_contain("yay testing");
        expect(output.str()).to_contain("lol test me");
        expect(output.str()).to_contain("other suite to run");
        expect(output.str()).to_contain("my tst");
        expect(output.str()).not_to_contain("do not run");
        expect(output.str()).not_to_contain("do not run this either");
    });

    test("run suites with wildcard at end", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            suite("my suite", [] {
                test("yay testing", [] {
                    expect(1).to_be(1);
                });

                test("lol test me", [] {
                    expect(1).to_be(1);
                });
            });

            suite("other suite to run", [] {
                test("my tst", [] {
                    expect(1).to_be(1);
                });
            });

            suite("do not run", [] {
                test("do not run this either", [] {
                    expect(1).to_be(1);
                });
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "--suite=other*"}.data()));
        }

        expect(output.str()).to_contain("Running 1 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 1 | 1 passed | 0 failed");
        expect(output.str()).not_to_contain("my suite");
        expect(output.str()).not_to_contain("yay testing");
        expect(output.str()).not_to_contain("lol test me");
        expect(output.str()).to_contain("other suite to run");
        expect(output.str()).to_contain("my tst");
        expect(output.str()).not_to_contain("do not run");
        expect(output.str()).not_to_contain("do not run this either");
    });

    test("run suites with wildcard in front", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            suite("my suite", [] {
                test("yay testing", [] {
                    expect(1).to_be(1);
                });

                test("lol test me", [] {
                    expect(1).to_be(1);
                });
            });

            suite("other suite to run", [] {
                test("my tst", [] {
                    expect(1).to_be(1);
                });
            });

            suite("do not run", [] {
                test("do not run this either", [] {
                    expect(1).to_be(1);
                });
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "--suite=*suite"}.data()));
        }

        expect(output.str()).to_contain("Running 2 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 2 | 2 passed | 0 failed");
        expect(output.str()).to_contain("my suite");
        expect(output.str()).to_contain("yay testing");
        expect(output.str()).to_contain("lol test me");
        expect(output.str()).not_to_contain("other suite to run");
        expect(output.str()).not_to_contain("my tst");
        expect(output.str()).not_to_contain("do not run");
        expect(output.str()).not_to_contain("do not run this either");
    });

    test("run suites with wildcards in front and back", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            suite("my suite", [] {
                test("yay testing", [] {
                    expect(1).to_be(1);
                });

                test("lol test me", [] {
                    expect(1).to_be(1);
                });
            });

            suite("other suite to run", [] {
                test("my tst", [] {
                    expect(1).to_be(1);
                });
            });

            suite("do not run", [] {
                test("do not run this either", [] {
                    expect(1).to_be(1);
                });
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "--suite=*suite*"}.data()));
        }

        expect(output.str()).to_contain("Running 3 tests from 2 test suites...");
        expect(output.str()).to_contain("Tests       : 3 | 3 passed | 0 failed");
        expect(output.str()).to_contain("my suite");
        expect(output.str()).to_contain("yay testing");
        expect(output.str()).to_contain("lol test me");
        expect(output.str()).to_contain("other suite to run");
        expect(output.str()).to_contain("my tst");
        expect(output.str()).not_to_contain("do not run");
        expect(output.str()).not_to_contain("do not run this either");
    });

    test("skip empty suites", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            suite("my suite", [] {
                test("yay testing", [] {
                    expect(1).to_be(1);
                });

                test("lol test me", [] {
                    expect(1).to_be(1);
                });
            });

            suite("other suite to run", [] {
                test("my tst", [] {
                    expect(1).to_be(1);
                });
            });

            suite("do not run", [] {
                test("do not run this either", [] {
                    expect(1).to_be(1);
                });
            });

            static_cast<void>(runner.run(2, std::array<const char *, 2>{"./app_test", "--test=\"my tst\""}.data()));
        }

        expect(output.str()).to_contain("Running 1 tests from 1 test suites...");
        expect(output.str()).to_contain("Tests       : 1 | 1 passed | 0 failed");
        expect(output.str()).not_to_contain("my suite");
        expect(output.str()).not_to_contain("yay testing");
        expect(output.str()).not_to_contain("lol test me");
        expect(output.str()).to_contain("other suite to run");
        expect(output.str()).to_contain("my tst");
        expect(output.str()).not_to_contain("do not run");
        expect(output.str()).not_to_contain("do not run this either");
    });
});
