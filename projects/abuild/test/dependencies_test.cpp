import atest;
import abuild;

using atest::expect;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Dependencies", [] {
    test("scan", [] {
        abuild::BuildCache cache;
        abuild::Settings settings{cache};
        abuild::Projects{cache, settings};
        abuild::Dependencies{cache};
    });
});
