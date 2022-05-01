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

static const auto S = suite("ModulePartition", [] { // NOLINT(cert-err58-cpp)
    test("add module", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path modulePath = "source.cpp";
        const std::filesystem::path partitionPath = "source.cpp";
        const ::abuild::File precompiledHeaderUnitFile{.path = "some/path/my_partition.pcm", .timestamp = 123};
        const std::string moduleName = "my_module";
        const std::string partitionName = "my_partition";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *moduleFile = cache.add_source_file(modulePath);
            assert_(moduleFile).not_to_be(nullptr);
            ::abuild::Module *mod = cache.add_module(moduleName);
            assert_(mod).not_to_be(nullptr);
            mod->sourceFile = moduleFile;

            ::abuild::SourceFile *partitionFile = cache.add_source_file(partitionPath);
            assert_(partitionFile).not_to_be(nullptr);
            ::abuild::ModulePartition *partition = cache.add_module_partition(partitionName);
            assert_(partition).not_to_be(nullptr);
            expect(partition->name).to_be(partitionName);
            partition->sourceFile = partitionFile;
            partition->mod = mod;
            partition->precompiledModuleInterface = precompiledHeaderUnitFile;
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *partitionFile = cache.exact_source_file(partitionPath);
        assert_(partitionFile).not_to_be(nullptr);
        ::abuild::Module *mod = cache.module_(moduleName);
        assert_(mod).not_to_be(nullptr);
        assert_(mod->partitions.size()).to_be(1U);
        assert_(mod->partitions[0]).not_to_be(nullptr);
        expect(mod->partitions[0]->name).to_be(partitionName);
        expect(mod->partitions[0]->mod).to_be(mod);
        expect(mod->partitions[0]->sourceFile).to_be(partitionFile);
        expect(mod->partitions[0]->precompiledModuleInterface).to_be(precompiledHeaderUnitFile);
    });
});
