import atest;
import abuild.cache;

using atest::expect;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::File", [] {
    test("construct with path", [] {
        const ::abuild::File file{"./main.cpp"};

        expect(file.extension()).to_be(".cpp");
        expect(file.path()).to_be(std::filesystem::path{"./main.cpp"});
        expect(file.is_outdated()).to_be(true);
    });

    test("construct outdated", [] {
        const ::abuild::File file{"./main.cpp", false};

        expect(file.extension()).to_be(".cpp");
        expect(file.path()).to_be(std::filesystem::path{"./main.cpp"});
        expect(file.is_outdated()).to_be(false);
    });
});
