import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

namespace abuild
{
auto operator<<(std::ostream &stream, const Flag &flag) -> std::ostream &
{
    stream << flag.name;

    if (!flag.value.empty())
    {
        stream << '=' << flag.value;
    }

    if (flag.affectsABI)
    {
        stream << " [abi]";
    }

    return stream;
}

[[nodiscard]] auto operator==(const Flag &left, const Flag &right) -> bool
{
    return left.name == right.name && left.value == right.value && left.affectsABI == right.affectsABI;
}
}

static const auto S = suite("Flag", [] { // NOLINT(cert-err58-cpp)
    test("compiler flags", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        const std::vector<::abuild::Flag> flags{
            {.name = "-f"},
            {           .name = "--flag",
             .value = "some_value"},
            { .name = "another_flag",
             .value = {},
             .affectsABI = true}
        };

        {
            ::abuild::Cache cache{file.path()};
            assert_(cache.compiler_flags()).to_be(std::vector<::abuild::Flag>{});
            cache.set_compiler_flags(std::vector<::abuild::Flag>{flags});
            expect(cache.compiler_flags()).to_be(flags);
        }

        ::abuild::Cache cache{file.path()};
        expect(cache.compiler_flags()).to_be(flags);
    });

    test("linker flags", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        const std::vector<::abuild::Flag> flags{
            {.name = "-f"},
            {           .name = "--flag",
             .value = "some_value"},
            { .name = "another_flag",
             .value = {},
             .affectsABI = true}
        };

        {
            ::abuild::Cache cache{file.path()};
            assert_(cache.linker_flags()).to_be(std::vector<::abuild::Flag>{});
            cache.set_linker_flags(std::vector<::abuild::Flag>{flags});
            expect(cache.linker_flags()).to_be(flags);
        }

        ::abuild::Cache cache{file.path()};
        expect(cache.linker_flags()).to_be(flags);
    });

    test("archiver flags", [] {
        const ::abuild::TestFile file{"./abuild.cache_test.yaml"};
        const std::vector<::abuild::Flag> flags{
            {.name = "-f"},
            {           .name = "--flag",
             .value = "some_value"},
            { .name = "another_flag",
             .value = {},
             .affectsABI = true}
        };

        {
            ::abuild::Cache cache{file.path()};
            assert_(cache.archiver_flags()).to_be(std::vector<::abuild::Flag>{});
            cache.set_archiver_flags(std::vector<::abuild::Flag>{flags});
            expect(cache.archiver_flags()).to_be(flags);
        }

        ::abuild::Cache cache{file.path()};
        expect(cache.archiver_flags()).to_be(flags);
    });
});
