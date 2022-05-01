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

static const auto S = suite("Module", [] { // NOLINT(cert-err58-cpp)
    test("add module", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "source.cpp";
        const ::abuild::File precompiledHeaderUnitFile{.path = "some/path/my_module.pcm", .timestamp = 123};
        const std::string name = "my_module";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *file = cache.add_source_file(path);
            assert_(file).not_to_be(nullptr);
            ::abuild::Module *mod = cache.add_module(name);
            assert_(mod).not_to_be(nullptr);
            expect(mod->name).to_be(name);
            expect(mod->exported).to_be(false);
            mod->sourceFile = file;
            mod->precompiledModuleInterface = precompiledHeaderUnitFile;
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *file = cache.exact_source_file(path);
        assert_(file).not_to_be(nullptr);
        ::abuild::Module *mod = cache.module_(name);
        assert_(mod).not_to_be(nullptr);
        expect(mod->sourceFile).to_be(file);
        expect(mod->name).to_be(name);
        expect(mod->exported).to_be(false);
        expect(mod->precompiledModuleInterface).to_be(precompiledHeaderUnitFile);
    });

    test("exported", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "source.cpp";
        const std::string name = "my_module";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *file = cache.add_source_file(path);
            assert_(file).not_to_be(nullptr);
            ::abuild::Module *mod = cache.add_module(name);
            assert_(mod).not_to_be(nullptr);
            mod->sourceFile = file;
            mod->exported = true;
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::Module *mod = cache.module_(name);
        assert_(mod).not_to_be(nullptr);
        expect(mod->exported).to_be(true);
    });

    test("module partitions", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "module.cpp";
        const std::filesystem::path partitionPath1 = "partition1.cpp";
        const std::filesystem::path partitionPath2 = "partition2.cpp";
        const std::filesystem::path partitionPath3 = "partition3.cpp";
        const std::string name = "my_module";
        const std::string partitionName1 = "my_partition1";
        const std::string partitionName2 = "my_partition2";
        const std::string partitionName3 = "my_partition3";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *file = cache.add_source_file(path);
            assert_(file).not_to_be(nullptr);
            ::abuild::Module *mod = cache.add_module(name);
            assert_(mod).not_to_be(nullptr);
            mod->sourceFile = file;

            ::abuild::SourceFile *partitionFile1 = cache.add_source_file(partitionPath1);
            ::abuild::SourceFile *partitionFile2 = cache.add_source_file(partitionPath2);
            ::abuild::SourceFile *partitionFile3 = cache.add_source_file(partitionPath3);
            assert_(partitionFile1).not_to_be(nullptr);
            assert_(partitionFile2).not_to_be(nullptr);
            assert_(partitionFile3).not_to_be(nullptr);
            ::abuild::ModulePartition *partition1 = cache.add_module_partition(partitionName1);
            ::abuild::ModulePartition *partition2 = cache.add_module_partition(partitionName2);
            ::abuild::ModulePartition *partition3 = cache.add_module_partition(partitionName3);
            assert_(partition1).not_to_be(nullptr);
            assert_(partition2).not_to_be(nullptr);
            assert_(partition3).not_to_be(nullptr);
            partition1->sourceFile = partitionFile1;
            partition2->sourceFile = partitionFile2;
            partition3->sourceFile = partitionFile3;

            mod->partitions = {partition1, partition2, partition3};
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::Module *mod = cache.module_(name);
        assert_(mod).not_to_be(nullptr);
        assert_(mod->partitions.size()).to_be(3U);
        assert_(mod->partitions[0]).not_to_be(nullptr);
        assert_(mod->partitions[1]).not_to_be(nullptr);
        assert_(mod->partitions[2]).not_to_be(nullptr);

        expect(mod->partitions[0]->name).to_be(partitionName1);
        expect(mod->partitions[1]->name).to_be(partitionName2);
        expect(mod->partitions[2]->name).to_be(partitionName3);
    });

});
