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

static const auto S = suite("SourceFile", [] { // NOLINT(cert-err58-cpp)
    test("add source file", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "source.cpp";
        const ::abuild::File objectFile = {.path = "some/path/source.cpp.obj", .timestamp = 123};

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *file = cache.add_source_file(path);
            assert_(file).not_to_be(nullptr);
            expect(file->path).to_be(path);
            file->objectFile = objectFile;
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *file = cache.exact_source_file(path);
        assert_(file).not_to_be(nullptr);
        expect(file->path).to_be(path);
        expect(file->objectFile).to_be(objectFile);
    });

    test("file project", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "source.cpp";
        const std::string projectName = "my_project";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *file = cache.add_source_file(path);
            ::abuild::Project *project = cache.add_project(projectName);
            assert_(file).not_to_be(nullptr);
            assert_(project).not_to_be(nullptr);
            file->project = project;
            project->sources.push_back(file);
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *file = cache.exact_source_file(path);
        assert_(file).not_to_be(nullptr);
        assert_(file->project).not_to_be(nullptr);
        expect(file->project->name).to_be(projectName);
    });

    test("includes", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "source.cpp";
        const std::filesystem::path includePath = "include.hpp";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *file = cache.add_source_file(path);
            ::abuild::HeaderFile *includeFile = cache.add_header_file(includePath);
            assert_(file).not_to_be(nullptr);
            assert_(includeFile).not_to_be(nullptr);
            file->includes.push_back(::abuild::Header{.file = includeFile});
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *file = cache.exact_source_file(path);
        ::abuild::HeaderFile *includeFile = cache.exact_header_file(includePath);
        assert_(file).not_to_be(nullptr);
        assert_(includeFile).not_to_be(nullptr);
        assert_(file->includes.size()).to_be(1U);
        expect(file->includes[0].file).to_be(includeFile);
    });

    test("imported header unit", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "source.cpp";
        const std::filesystem::path headerUnitPath = "header_unit.hpp";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *file = cache.add_source_file(path);
            ::abuild::HeaderFile *headerUnitFile = cache.add_header_file(headerUnitPath);
            assert_(headerUnitFile).not_to_be(nullptr);
            ::abuild::HeaderUnit *headerUnit = cache.add_header_unit(headerUnitFile);
            assert_(file).not_to_be(nullptr);
            assert_(headerUnit).not_to_be(nullptr);
            file->importedHeaderUnits.push_back(headerUnit);
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *file = cache.exact_source_file(path);
        ::abuild::HeaderFile *headerUnitFile = cache.exact_header_file(headerUnitPath);
        assert_(headerUnitFile).not_to_be(nullptr);
        ::abuild::HeaderUnit *headerUnit = cache.header_unit(headerUnitFile);
        assert_(file).not_to_be(nullptr);
        assert_(headerUnit).not_to_be(nullptr);
        expect(file->importedHeaderUnits).to_be(std::vector<::abuild::HeaderUnit *>{headerUnit});
    });

    test("imported module", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "source.cpp";
        const std::filesystem::path modPath = "mod.cpp";
        const std::string moduleName = "my_module";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *file = cache.add_source_file(path);
            ::abuild::SourceFile *modFile = cache.add_source_file(modPath);
            ::abuild::Module *mod = cache.add_module(moduleName);
            assert_(file).not_to_be(nullptr);
            assert_(mod).not_to_be(nullptr);
            assert_(modFile).not_to_be(nullptr);
            mod->sourceFile = modFile;
            file->importedModules.push_back(mod);
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *file = cache.exact_source_file(path);
        ::abuild::Module *mod = cache.module_(moduleName);
        assert_(file).not_to_be(nullptr);
        assert_(mod).not_to_be(nullptr);
        expect(file->importedModules).to_be(std::vector<::abuild::Module *>{mod});
    });

    test("imported module partition", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "source.cpp";
        const std::filesystem::path modPath = "mod.cpp";
        const std::filesystem::path partitionPath = "partition.cpp";
        const std::string moduleName = "my_module";
        const std::string partitionName = "my_partition";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *file = cache.add_source_file(path);
            ::abuild::Module *mod = cache.add_module(moduleName);
            ::abuild::SourceFile *modFile = cache.add_source_file(modPath);
            ::abuild::SourceFile *partitionFile = cache.add_source_file(partitionPath);
            ::abuild::ModulePartition *partition = cache.add_module_partition(partitionName);
            assert_(file).not_to_be(nullptr);
            assert_(mod).not_to_be(nullptr);
            assert_(partition).not_to_be(nullptr);
            assert_(modFile).not_to_be(nullptr);
            assert_(partitionFile).not_to_be(nullptr);
            mod->partitions.push_back(partition);
            mod->sourceFile = modFile;
            partition->mod = mod;
            partition->sourceFile = partitionFile;
            file->importedModulePartitions.push_back(partition);
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *file = cache.exact_source_file(path);
        ::abuild::Module *mod = cache.module_(moduleName);
        assert_(mod).not_to_be(nullptr);
        assert_(mod->partitions.size()).to_be(1U);
        ::abuild::ModulePartition *partition = mod->partitions[0];
        assert_(file).not_to_be(nullptr);
        assert_(partition).not_to_be(nullptr);
        expect(file->importedModulePartitions).to_be(std::vector<::abuild::ModulePartition *>{partition});
    });
});
