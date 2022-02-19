import atest;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

struct MyStruct
{
    int value = 0; // NOLINT(misc-non-private-member-variables-in-classes)

    [[nodiscard]] auto operator==(const MyStruct &other) const noexcept -> bool
    {
        return value == other.value;
    }
};

auto operator<<(std::ostream &stream, const MyStruct &value) -> std::ostream &
{
    stream << "MyStruct{" << value.value << '}';
    return stream;
}

static const auto S = suite("Expect::to_be()", [] { // NOLINT(cert-err58-cpp)
    test("expect().to_be()", [] {
        expect(1).to_be(1);
    });

    test("expect(callable).to_be()", [] {
        expect([] { return 1 + 1; }).to_be(2);
    });

    test("expect(string).to_be()", [] {
        expect(std::string{}).to_be("");
        expect("").to_be(std::string{});
    });

    test("expect().not_to_be()", [] {
        expect(1).not_to_be(2);
    });

    test("expect(custom type).not_to_be()", [] {
        expect(MyStruct{1}).not_to_be(MyStruct{2});
    });

    test("expect(std::vector).not_to_be()", [] {
        expect(std::vector<int>{1, 2, 3}).not_to_be(std::vector<int>{3, 2, 1});
    });

    test("expect(std::vector<custom type>).to_be()", [] {
        expect(std::vector<MyStruct>{{1}, {2}, {3}}).not_to_be(std::vector<MyStruct>{{3}, {2}, {1}});
    });

    test("expect(std::pair).not_to_be()", [] {
        expect(std::pair<int, int>{1, 2}).not_to_be(std::pair<int, int>{2, 1});
    });

    test("expect().to_be() (failing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect(1).to_be(2);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Values do not match.");
        expect(output.str()).to_contain("Expected:  2");
        expect(output.str()).to_contain("Actual  :  1");
    });

    test("expect().not_to_be() (passing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect(1).not_to_be(1);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Expected a failure but the expectation passed.");
    });

    test("expect().not_to_be() (throwing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect([]() -> int { throw std::runtime_error{"bad call"}; }).not_to_be(1);
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Unexpected exception thrown (" + std::string{typeid(std::runtime_error).name()} + "): bad call");
    });

    test("expect().not_to_be() (throw int)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect([]() -> int { throw 1; }).not_to_be(1); // NOLINT(hicpp-exception-baseclass)
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Unexpected exception thrown");
    });

    test("expect(std::vector).to_be() (failing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect(std::vector<int>{1, 2, 3}).to_be(std::vector<int>{3, 2, 1});
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Expected:  {3, 2, 1}");
        expect(output.str()).to_contain("Actual  :  {1, 2, 3}");
    });

    test("expect(custom type).to_be() (failing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                expect(std::vector<MyStruct>{{1}, {2}, {3}}).to_be(std::vector<MyStruct>{{3}, {2}, {1}});
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).to_contain("Expected:  {MyStruct{3}, MyStruct{2}, MyStruct{1}}");
        expect(output.str()).to_contain("Actual  :  {MyStruct{1}, MyStruct{2}, MyStruct{3}}");
    });
});
