import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("file lookup", [] { // NOLINT(cert-err58-cpp)
    test("several similar headers", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{testFile.path()};

        cache.add_header_file("c:/dev/my_project/include/my_header.hpp");
        cache.add_header_file("c:/dev/my_project/my_header.hpp");
        cache.add_header_file("c:/dev/my_other_project/my_header.hpp");
        cache.add_header_file("c:/dev/my_other_project/include/my_header.hpp");

        ::abuild::HeaderFile *file = cache.header_file("my_other_project/my_header.hpp");
        assert_(file).not_to_be(nullptr);
        expect(file->path).to_be("c:/dev/my_other_project/my_header.hpp");
    });

    test("missing header", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{testFile.path()};

        cache.add_header_file("c:/dev/my_project/include/my_header.hpp");
        cache.add_header_file("c:/dev/my_project/my_header.hpp");
        cache.add_header_file("c:/dev/my_other_project/my_header.hpp");
        cache.add_header_file("c:/dev/my_other_project/include/my_header.hpp");

        ::abuild::HeaderFile *file = cache.header_file("different_project/my_header.hpp");
        expect(file).to_be(nullptr);
    });

    test("several similar sources", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{testFile.path()};

        cache.add_source_file("c:/dev/my_project/main.cpp");
        cache.add_source_file("c:/dev/my_project/src/main.cpp");
        cache.add_source_file("c:/dev/my_other_project/main.cpp");
        cache.add_source_file("c:/dev/my_other_project/src/main.cpp");

        ::abuild::SourceFile *file = cache.source_file("my_other_project/main.cpp");
        assert_(file).not_to_be(nullptr);
        expect(file->path).to_be("c:/dev/my_other_project/main.cpp");
    });

    test("missing source", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{testFile.path()};

        cache.add_source_file("c:/dev/my_project/main.cpp");
        cache.add_source_file("c:/dev/my_project/src/main.cpp");
        cache.add_source_file("c:/dev/my_other_project/main.cpp");
        cache.add_source_file("c:/dev/my_other_project/src/main.cpp");

        ::abuild::SourceFile *file = cache.source_file("different_project/main.cpp");
        expect(file).to_be(nullptr);
    });

    test("missing exact source", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{testFile.path()};

        ::abuild::SourceFile *file = cache.exact_source_file("c:/dev/my_project/main.cpp");
        expect(file).to_be(nullptr);
    });

    test("missing exact header", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{testFile.path()};

        ::abuild::HeaderFile *file = cache.exact_header_file("c:/dev/my_project/header.hpp");
        expect(file).to_be(nullptr);
    });
});
