import atest;

using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Expect::to_contain()", [] { // NOLINT(cert-err58-cpp)
    test("expect().to_contain()", [] {
        expect(std::vector<std::string>{"Hello", ",", "World", "!"}).to_contain("World");
    });

    test("expect(string).to_contain()", [] {
        expect("Hello, World!").to_contain("World");
    });

    test("expect_fail().to_contain()", [] {
        expect_fail(std::vector<int>{1, 2, 3}).to_contain(4);
    });

    test("expect_fail(string).to_contain()", [] {
        expect_fail("Hello, World!").to_contain("Hi");
    });

    test("expect().to_contain() (failing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect(std::vector<std::string>{"Hello", ",", "World", "!"}).to_contain("Hi");
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Expected is not contained in Actual");
        expect(output.str()).to_contain("Expected:  Hi");
        expect(output.str()).to_contain("Actual  :  {Hello, ,, World, !}");
    });

    test("expect(string).to_contain() (failing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect("Hello, World!").to_contain("Hi");
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Expected is not contained in Actual");
        expect(output.str()).to_contain("Expected:  Hi");
        expect(output.str()).to_contain("Actual  :  Hello, World!");
    });
});
