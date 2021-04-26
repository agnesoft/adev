import atest;
import abuild;

using atest::expect;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Toolchains", [] {
    test("type traits", [] {

    });

    test("scan", [] {
        abuild::BuildCache cache;
        abuild::Toolchains{cache};
    });
});
