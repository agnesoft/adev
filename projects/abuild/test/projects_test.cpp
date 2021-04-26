import atest;
import abuild;

using atest::expect;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::ProjectScanner", [] {
    test("type traits", [] {

    });

    test("scan", [] {
        abuild::BuildCache cache;
        abuild::Settings{cache};
        abuild::ProjectScanner{cache};
    });
});
