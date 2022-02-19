import atest;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("assert_()", [] { // NOLINT(cert-err58-cpp)
    test("assert_().to_be()", [] {
        assert_(1).to_be(1);
    });

    test("assert_().to_contain()", [] {
        assert_(std::vector<int>{1, 2, 3, 4}).to_contain(3);
    });

    test("assert_().to_throw()", [] {
        assert_([] { throw 1; }).to_throw<int>(); // NOLINT(hicpp-exception-baseclass)
    });

    test("assert_().not_to_be()", [] {
        assert_(1).not_to_be(2);
    });

    test("assert_().not_to_contain()", [] {
        assert_(std::vector<int>{1, 2, 3, 4}).not_to_contain(0);
    });

    test("assert_().not_to_throw()", [] {
        assert_([] {}).not_to_throw<int>();
    });

    test("assert stops the test", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                assert_(1).to_be(2);
                throw std::logic_error{"This should not happen"};
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).not_to_contain("This should not happen");
    });

    test("assert stops the test (throwing)", [] {
        std::stringstream output;

        {
            ::atest::TestRunner runner{output};

            test("test", [] {
                assert_([] {}).to_throw<int>();
                throw std::logic_error{"This should not happen too"};
            });

            static_cast<void>(runner.run(0, nullptr));
        }

        expect(output.str()).not_to_contain("This should not happen too");
    });
});
