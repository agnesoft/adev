import atest;
import abuild.cache;

using atest::expect;
using atest::suite;
using atest::test;

static const auto S = suite("abuild::SourceFileBase", [] { // NOLINT(cert-err58-cpp)
    test("construct with path", [] {
        const ::abuild::SourceFileBase file{"./main.cpp"};

        expect(file.is_outdated()).to_be(true);
        expect(file.tokens()).to_be(std::vector<::abuild::Token>{});
    });

    test("construct with tokens", [] {
        const ::abuild::SourceFileBase file{"./main.cpp", std::vector<::abuild::Token>{::abuild::IfToken{}, ::abuild::EndIfToken{}}};

        expect(file.is_outdated()).to_be(false);
        expect(file.tokens()).to_be(std::vector<::abuild::Token>{::abuild::IfToken{}, ::abuild::EndIfToken{}});
    });
});
