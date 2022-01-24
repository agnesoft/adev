import atest;
import abuild.cache;
import abuild.test_utilities;

using atest::expect;
using atest::suite;
using atest::test;

static const auto S = suite("abuild::SourceFile", [] { // NOLINT(cert-err58-cpp)
    test("construct with tokens", [] {
        const ::abuild::SourceFile file{"./main.cpp",
                                        std::vector<::abuild::Token>{::abuild::IfToken{}, ::abuild::EndIfToken{}},
                                        false};

        expect(file.is_outdated()).to_be(false);
        expect(file.tokens()).to_be(std::vector<::abuild::Token>{::abuild::IfToken{}, ::abuild::EndIfToken{}});
    });
});
