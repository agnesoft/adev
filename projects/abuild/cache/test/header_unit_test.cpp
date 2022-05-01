import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

namespace abuild
{
auto operator<<(std::ostream &stream, const File &file) -> std::ostream &;
auto operator==(const File &left, const File &right) -> bool;
}

static const auto S = suite("HeaderUnit", [] { // NOLINT(cert-err58-cpp)
    test("add header unit", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const ::abuild::File precompiledHeaderUnitFile{.path = "some/path/header.hpp.ifc", .timestamp = 123};

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            assert_(file).not_to_be(nullptr);
            ::abuild::HeaderUnit *unit = cache.add_header_unit(file);
            assert_(unit).not_to_be(nullptr);
            expect(unit->headerFile).to_be(file);
            unit->precompiledHeaderUnit = precompiledHeaderUnitFile;
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::HeaderFile *file = cache.exact_header_file(path);
        assert_(file).not_to_be(nullptr);
        ::abuild::HeaderUnit *unit = cache.header_unit(file);
        assert_(unit).not_to_be(nullptr);
        expect(unit->headerFile).to_be(file);
        expect(unit->precompiledHeaderUnit).to_be(precompiledHeaderUnitFile);
    });
});
