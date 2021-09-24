import atest;

using ::atest::suite;
using ::atest::test;

static const auto S1 = suite("Bad test suite that throws an int (INTENTIONAL FAILURE)", [] { //NOLINT(cert-err58-cpp)
    throw 1; //NOLINT(hicpp-exception-baseclass)
});

static const auto S2 = suite("Bad test suite that throws std::logic_error (INTENTIONAL FAILURE)", [] { //NOLINT(cert-err58-cpp)
    throw std::logic_error{"Really bad test suite"};
});

static const auto S3 = suite("Bad test suite", [] { //NOLINT(cert-err58-cpp)
    test("Bad test that throws an int (INTENTIONAL FAILURE)", [] {
        throw 1; //NOLINT(hicpp-exception-baseclass)
    });

    test("Bad test that throws std::logic_error (INTENTIONAL FAILURE)", [] {
        throw std::logic_error{"Really bad test"};
    });
});
