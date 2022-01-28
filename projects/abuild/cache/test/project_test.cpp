import atest;
import abuild.cache;
import abuild.test_utilities;

using atest::expect;
using atest::suite;
using atest::test;

static const auto S = suite("abuild::Project", [] { // NOLINT(cert-err58-cpp)
    test("construct with name", [] {
        ::abuild::Project project{"myproject"};

        expect(project.name()).to_be("myproject");
    });

    test("add source file", [] {
        ::abuild::SourceFile source1{"./my_source.cpp", {}, true};
        ::abuild::SourceFile source2{"./other_source.cpp", {}, false};
        ::abuild::Project project{"myproject"};

        project.add_source_file(&source1);
        project.add_source_file(&source2);

        expect(project.source_files()).to_be(std::vector<::abuild::SourceFile *>{&source1, &source2});
    });

    test("add header file", [] {
        ::abuild::HeaderFile header1{"./my_header.hpp", {}, true};
        ::abuild::HeaderFile header2{"./other_header.hpp", {}, false};
        ::abuild::Project project{"myproject"};

        project.add_header_file(&header1);
        project.add_header_file(&header2);

        expect(project.header_files()).to_be(std::vector<::abuild::HeaderFile *>{&header1, &header2});
    });
});
