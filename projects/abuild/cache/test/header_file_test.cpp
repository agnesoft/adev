import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("HeaderFile", [] { // NOLINT(cert-err58-cpp)
    test("add header file", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";

        {
            ::abuild::Cache cache{testFile.path()};
            const ::abuild::HeaderFile *file = cache.add_header_file(path);
            assert_(file).not_to_be(nullptr);
            expect(file->path).to_be(path);
        }

        ::abuild::Cache cache{testFile.path()};
        const ::abuild::HeaderFile *file = cache.exact_header_file(path);
        assert_(file).not_to_be(nullptr);
        expect(file->path).to_be(path);
    });

    test("file project", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::string projectName = "my_project";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            ::abuild::Project *project = cache.add_project(projectName);
            assert_(file).not_to_be(nullptr);
            assert_(project).not_to_be(nullptr);
            file->project = project;
            project->headers.push_back(file);
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::HeaderFile *file = cache.exact_header_file(path);
        assert_(file).not_to_be(nullptr);
        assert_(file->project).not_to_be(nullptr);
        expect(file->project->name).to_be(projectName);
    });

    test("includes", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::filesystem::path includePath = "include.hpp";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            ::abuild::HeaderFile *includeFile = cache.add_header_file(includePath);
            assert_(file).not_to_be(nullptr);
            assert_(includeFile).not_to_be(nullptr);
            file->includes.push_back(::abuild::Header{.file = includeFile});
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::HeaderFile *file = cache.exact_header_file(path);
        ::abuild::HeaderFile *includeFile = cache.exact_header_file(includePath);
        assert_(file).not_to_be(nullptr);
        assert_(includeFile).not_to_be(nullptr);
        assert_(file->includes.size()).to_be(1U);
        expect(file->includes[0].file).to_be(includeFile);
    });

    test("imported header unit", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::filesystem::path headerUnitPath = "header_unit.hpp";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            ::abuild::HeaderFile *headerUnitFile = cache.add_header_file(headerUnitPath);
            assert_(headerUnitFile).not_to_be(nullptr);
            ::abuild::HeaderUnit *headerUnit = cache.add_header_unit(headerUnitFile);
            assert_(file).not_to_be(nullptr);
            assert_(headerUnit).not_to_be(nullptr);
            file->importedHeaderUnits.push_back(headerUnit);
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::HeaderFile *file = cache.exact_header_file(path);
        ::abuild::HeaderFile *headerUnitFile = cache.exact_header_file(headerUnitPath);
        assert_(headerUnitFile).not_to_be(nullptr);
        ::abuild::HeaderUnit *headerUnit = cache.header_unit(headerUnitFile);
        assert_(file).not_to_be(nullptr);
        assert_(headerUnit).not_to_be(nullptr);
        expect(file->importedHeaderUnits).to_be(std::vector<::abuild::HeaderUnit *>{headerUnit});
    });

    test("imported module", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::filesystem::path modPath = "mod.cpp";
        const std::string moduleName = "my_module";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            ::abuild::SourceFile *modFile = cache.add_source_file(modPath);
            ::abuild::Module *mod = cache.add_module(moduleName);
            assert_(file).not_to_be(nullptr);
            assert_(mod).not_to_be(nullptr);
            assert_(modFile).not_to_be(nullptr);
            mod->sourceFile = modFile;
            file->importedModules.push_back(mod);
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::HeaderFile *file = cache.exact_header_file(path);
        ::abuild::Module *mod = cache.module_(moduleName);
        assert_(file).not_to_be(nullptr);
        assert_(mod).not_to_be(nullptr);
        expect(file->importedModules).to_be(std::vector<::abuild::Module *>{mod});
    });

    test("imported module partition", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::filesystem::path modPath = "mod.cpp";
        const std::filesystem::path partitionPath1 = "partition1.cpp";
        const std::filesystem::path partitionPath2 = "partition2.cpp";
        const std::filesystem::path partitionPath3 = "partition3.cpp";
        const std::string moduleName = "my_module";
        const std::string partitionName1 = "my_partition1";
        const std::string partitionName2 = "my_partition2";
        const std::string partitionName3 = "my_partition3";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            ::abuild::SourceFile *modFile = cache.add_source_file(modPath);
            ::abuild::SourceFile *partitionFile1 = cache.add_source_file(partitionPath1);
            ::abuild::SourceFile *partitionFile2 = cache.add_source_file(partitionPath2);
            ::abuild::SourceFile *partitionFile3 = cache.add_source_file(partitionPath3);
            ::abuild::Module *mod = cache.add_module(moduleName);
            ::abuild::ModulePartition *partition1 = cache.add_module_partition(partitionName1);
            ::abuild::ModulePartition *partition2 = cache.add_module_partition(partitionName2);
            ::abuild::ModulePartition *partition3 = cache.add_module_partition(partitionName3);
            assert_(file).not_to_be(nullptr);
            assert_(mod).not_to_be(nullptr);
            assert_(partition1).not_to_be(nullptr);
            assert_(partition2).not_to_be(nullptr);
            assert_(partition3).not_to_be(nullptr);
            assert_(modFile).not_to_be(nullptr);
            assert_(partitionFile1).not_to_be(nullptr);
            assert_(partitionFile2).not_to_be(nullptr);
            assert_(partitionFile3).not_to_be(nullptr);
            mod->partitions.push_back(partition1);
            mod->partitions.push_back(partition2);
            mod->partitions.push_back(partition3);
            mod->sourceFile = modFile;
            partition1->mod = mod;
            partition2->mod = mod;
            partition3->mod = mod;
            partition1->sourceFile = partitionFile1;
            partition2->sourceFile = partitionFile2;
            partition3->sourceFile = partitionFile3;
            file->importedModulePartitions.push_back(partition2);
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::HeaderFile *file = cache.exact_header_file(path);
        ::abuild::Module *mod = cache.module_(moduleName);
        assert_(mod).not_to_be(nullptr);
        assert_(mod->partitions.size()).to_be(3U);
        ::abuild::ModulePartition *partition = mod->partitions[1];
        assert_(file).not_to_be(nullptr);
        assert_(partition).not_to_be(nullptr);
        expect(file->importedModulePartitions).to_be(std::vector<::abuild::ModulePartition *>{partition});
    });

    test("missing header file", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const ::abuild::Cache cache{testFile.path()};
        expect(cache.exact_header_file("missing/file.hpp")).to_be(nullptr);
    });

    test("imported missing module", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::filesystem::path modPath = "mod.cpp";
        const std::string moduleName = "my_module";

        {
            ::abuild::Module mod{.name = moduleName};

            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            ::abuild::SourceFile *modFile = cache.add_source_file(modPath);
            assert_(file).not_to_be(nullptr);
            assert_(modFile).not_to_be(nullptr);
            mod.sourceFile = modFile;
            file->importedModules.push_back(&mod);
        }

        expect([&] { ::abuild::Cache{testFile.path()}; }).to_throw<std::runtime_error>("Corrupted cache: missing imported module '" + moduleName + '\'');
    });

    test("imported missing header unit", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::filesystem::path headerUnitPath = "header_unit.hpp";

        {
            ::abuild::HeaderUnit headerUnit;
            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            assert_(file).not_to_be(nullptr);
            ::abuild::HeaderFile *headerUnitFile = cache.add_header_file(headerUnitPath);
            assert_(headerUnitFile).not_to_be(nullptr);
            headerUnit.headerFile = headerUnitFile;
            file->importedHeaderUnits.push_back(&headerUnit);
        }

        expect([&] { ::abuild::Cache{testFile.path()}; }).to_throw<std::runtime_error>("Corrupted cache: missing imported header unit '" + headerUnitPath.string() + '\'');
    });

    test("imported missing header unit file", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::filesystem::path headerUnitPath = "header_unit.hpp";

        {
            ::abuild::HeaderFile headerUnitFile;
            headerUnitFile.path = headerUnitPath;
            ::abuild::HeaderUnit headerUnit;
            headerUnit.headerFile = &headerUnitFile;

            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            assert_(file).not_to_be(nullptr);
            file->importedHeaderUnits.push_back(&headerUnit);
        }

        expect([&] { ::abuild::Cache{testFile.path()}; }).to_throw<std::runtime_error>("Corrupted cache: missing imported header unit file '" + headerUnitPath.string() + '\'');
    });

    test("imported missing module partition's module", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::filesystem::path modPath = "mod.cpp";
        const std::filesystem::path partitionPath = "partition.cpp";
        const std::string moduleName = "my_module";
        const std::string partitionName = "my_partition";

        {
            ::abuild::Module mod{.name = moduleName};

            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            ::abuild::SourceFile *modFile = cache.add_source_file(modPath);
            ::abuild::SourceFile *partitionFile = cache.add_source_file(partitionPath);
            ::abuild::ModulePartition *partition = cache.add_module_partition(partitionName);
            assert_(file).not_to_be(nullptr);
            assert_(partition).not_to_be(nullptr);
            assert_(modFile).not_to_be(nullptr);
            assert_(partitionFile).not_to_be(nullptr);
            mod.partitions.push_back(partition);
            mod.sourceFile = modFile;
            partition->mod = &mod;
            partition->sourceFile = partitionFile;
            file->importedModulePartitions.push_back(partition);
        }

        expect([&] { ::abuild::Cache{testFile.path()}; }).to_throw<std::runtime_error>("Corrupted cache: missing imported module '" + moduleName + '\'');
    });

    test("imported missing module partition", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path path = "header.hpp";
        const std::filesystem::path modPath = "mod.cpp";
        const std::filesystem::path partitionPath = "partition.cpp";
        const std::string moduleName = "my_module";
        const std::string partitionName = "my_partition";

        {
            ::abuild::ModulePartition partition{.name = partitionName};

            ::abuild::Cache cache{testFile.path()};
            ::abuild::HeaderFile *file = cache.add_header_file(path);
            ::abuild::SourceFile *modFile = cache.add_source_file(modPath);
            ::abuild::SourceFile *partitionFile = cache.add_source_file(partitionPath);
            ::abuild::Module *mod = cache.add_module(moduleName);
            assert_(file).not_to_be(nullptr);
            assert_(mod).not_to_be(nullptr);
            assert_(modFile).not_to_be(nullptr);
            assert_(partitionFile).not_to_be(nullptr);
            mod->sourceFile = modFile;
            partition.mod = mod;
            partition.sourceFile = partitionFile;
            file->importedModulePartitions.push_back(&partition);
        }

        expect([&] { ::abuild::Cache{testFile.path()}; }).to_throw<std::runtime_error>("Corrupted cache: missing imported module partition '" + moduleName + ':' + partitionName + '\'');
    });
});
