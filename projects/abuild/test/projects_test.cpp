import atest;
import abuild;

using atest::expect;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Projects", [] {
    test("scan", [] {
        abuild::BuildCache cache;
        abuild::Settings settings{cache};
        abuild::Projects{cache, settings};
    });
});
