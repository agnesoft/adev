import atest;

using atest::suite;
using atest::test;

static const auto s1 = suite("Bad test suite that throws an int (INTENTIONAL FAILURE)", [] { //NOLINT(cert-err58-cpp)
    throw 1;
});

static const auto s2 = suite("Bad test suite that throws std::logic_error (INTENTIONAL FAILURE)", [] { //NOLINT(cert-err58-cpp)
    throw std::logic_error{"Really bad test suite"};
});

static const auto s3 = suite("Bad test suite", [] { //NOLINT(cert-err58-cpp)
    test("Bad test that throws an int (INTENTIONAL FAILURE)", [] {
        throw 1;
    });

    test("Bad test that throws std::logic_error (INTENTIONAL FAILURE)", [] {
        throw std::logic_error{"Really bad test"};
    });
});
