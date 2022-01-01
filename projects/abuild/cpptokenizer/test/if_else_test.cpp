import atest;
import abuild.cpptokenizer;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("if else", [] { // NOLINT(cert-err58-cpp)
    test("#else", [] {

    });

    test("#elif", [] {

    });

    test("#elifdef", [] {

    });

    test("#elifndef", [] {

    });
});
