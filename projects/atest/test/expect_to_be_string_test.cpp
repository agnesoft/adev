import atest;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Expect::to_be() - string", [] { // NOLINT(cert-err58-cpp)
    test("short string same size", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                const char *actual = "My very long string that is alomst the same as the actual one.";
                std::string expected{"My very long string that is almost the same as the actual one."};

                expect(actual).to_be(expected);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("  Expected:  (62) My very long string that is almost the same as the actual one.\n"
                                        "  Actual  :  (62) My very long string that is alomst the same as the actual one.\n"
                                        "                                                ^30");
    });

    test("short string different size", [] {
        std::stringstream output;
        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                const char *actual = "My very long string that is almost the same as the actual one.";
                std::string expected{"My very long string that is"};

                expect(actual).to_be(expected);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("  Expected:  (27) My very long string that is\n"
                                        "  Actual  :  (62) My very long string that is almost the same as the actual one.\n"
                                        "                                             ^27");
    });

    test("short string vs empty string", [] {
        std::stringstream output;
        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                const char *actual = "";
                std::string expected{"My short string"};

                expect(actual).to_be(expected);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("  Expected:  (15) My short string\n"
                                        "  Actual  :  (0)  \n"
                                        "                  ^0");
    });

    test("long string truncates both ends", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                std::string expected{"My very long string that is longer than allowed length of the string to be displayed; I mean really much too long to be displayed so it has to be truncated but somewhere down the line there is a slight error that is hard to discern unless the helpful matcher points it out."};
                const char *actual = "My very long string that is longer than allowed length of the string to be displayed; I mean really much too long to be displayed so it has to be truncated but somewhere down the line there is a slight eror that is hard to discern unless the helpful matcher points it out.";

                expect(actual).to_be(expected);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("  Expected:  (273) [...] where down the line there is a slight error that is hard to discern unless the h [...]\n"
                                        "  Actual  :  (272) [...] where down the line there is a slight eror that is hard to discern unless the he [...]\n"
                                        "                                                                 ^204");
    });

    test("long string differs at position 0", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                std::string expected{"My very long string that is longer than allowed length of the string to be displayed; I mean really much too long to be displayed so it has to be truncated but somewhere down the line there is a slight error that is hard to discern unless the helpful matcher points it out."};
                const char *actual = "y very long string that is longer than allowed length of the string to be displayed; I mean really much too long to be displayed so it has to be truncated but somewhere down the line there is a slight error that is hard to discern unless the helpful matcher points it out.";

                expect(actual).to_be(expected);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("  Expected:  (273) My very long string that is longer than allowed length of the string to be displ [...]\n"
                                        "  Actual  :  (272) y very long string that is longer than allowed length of the string to be displa [...]\n"
                                        "                   ^0");
    });

    test("long string differs at last position", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                std::string expected{"My very long string that is longer than allowed length of the string to be displayed; I mean really much too long to be displayed so it has to be truncated but somewhere down the line there is a slight error that is hard to discern unless the helpful matcher points it out."};
                const char *actual = "My very long string that is longer than allowed length of the string to be displayed; I mean really much too long to be displayed so it has to be truncated but somewhere down the line there is a slight error that is hard to discern unless the helpful matcher points it out";

                expect(actual).to_be(expected);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("  Expected:  (273) [...] unless the helpful matcher points it out.\n"
                                        "  Actual  :  (272) [...] unless the helpful matcher points it out\n"
                                        "                                                                 ^272");
    });

    test("long string vs empty string", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                std::string expected;
                const char *actual = "My very long string that is longer than allowed length of the string to be displayed; I mean really much too long to be displayed so it has to be truncated but somewhere down the line there is a slight error that is hard to discern unless the helpful matcher points it out";

                expect(actual).to_be(expected);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("  Expected:  (0)   \n"
                                        "  Actual  :  (272) My very long string that is longer than allowed length of the string to be displ [...]\n"
                                        "                   ^0");
    });

    test("long string differs at position 30", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                std::string expected{"My very long string that is longer than allowed length of the string to be displayed; I mean really much too long to be displayed so it has to be truncated but somewhere down the line there is a slight error that is hard to discern unless the helpful matcher points it out."};
                const char *actual = "My very long string that is lnger than allowed length of the string to be displayed; I mean";

                expect(actual).to_be(expected);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("  Expected:  (273) My very long string that is longer than allowed length of the string to be displ [...]\n"
                                        "  Actual  :  (91)  My very long string that is lnger than allowed length of the string to be displa [...]\n"
                                        "                                                ^29");
    });
});
