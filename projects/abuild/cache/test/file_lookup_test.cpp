import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("file lookup", [] { // NOLINT(cert-err58-cpp)
    test("several similar headers", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{file.path()};

        cache.add_header_file("c:/dev/my_project/include/my_header.hpp", "my_project");
        cache.add_header_file("c:/dev/my_project/my_header.hpp", "my_project");
        cache.add_header_file("c:/dev/my_other_project/my_header.hpp", "my_project");
        cache.add_header_file("c:/dev/my_other_project/include/my_header.hpp", "my_project");

        ::abuild::HeaderFile *f = cache.header_file("my_other_project/my_header.hpp");
        assert_(f).not_to_be(nullptr);
        expect(f->path).to_be("c:/dev/my_other_project/my_header.hpp");
    });

    test("missing header", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{file.path()};

        cache.add_header_file("c:/dev/my_project/include/my_header.hpp", "my_project");
        cache.add_header_file("c:/dev/my_project/my_header.hpp", "my_project");
        cache.add_header_file("c:/dev/my_other_project/my_header.hpp", "my_project");
        cache.add_header_file("c:/dev/my_other_project/include/my_header.hpp", "my_project");

        ::abuild::HeaderFile *f = cache.header_file("different_project/my_header.hpp");
        expect(f).to_be(nullptr);
    });

    test("several similar sources", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{file.path()};

        cache.add_source_file("c:/dev/my_project/main.cpp", "my_project");
        cache.add_source_file("c:/dev/my_project/src/main.cpp", "my_project");
        cache.add_source_file("c:/dev/my_other_project/main.cpp", "my_project");
        cache.add_source_file("c:/dev/my_other_project/src/main.cpp", "my_project");

        ::abuild::SourceFile *f = cache.source_file("my_other_project/main.cpp");
        assert_(f).not_to_be(nullptr);
        expect(f->path).to_be("c:/dev/my_other_project/main.cpp");
    });

    test("missing source", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{file.path()};

        cache.add_source_file("c:/dev/my_project/main.cpp", "my_project");
        cache.add_source_file("c:/dev/my_project/src/main.cpp", "my_project");
        cache.add_source_file("c:/dev/my_other_project/main.cpp", "my_project");
        cache.add_source_file("c:/dev/my_other_project/src/main.cpp", "my_project");

        ::abuild::SourceFile *f = cache.source_file("different_project/main.cpp");
        expect(f).to_be(nullptr);
    });

    test("missing exact source", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{file.path()};

        ::abuild::SourceFile *f = cache.exact_source_file("c:/dev/my_project/main.cpp");
        expect(f).to_be(nullptr);
    });

    test("missing exact header", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        ::abuild::Cache cache{file.path()};

        ::abuild::HeaderFile *f = cache.exact_header_file("c:/dev/my_project/header.hpp");
        expect(f).to_be(nullptr);
    });
});
