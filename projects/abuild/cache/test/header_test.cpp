import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

namespace abuild
{
auto operator<<(std::ostream &stream, const Header &header) -> std::ostream &
{
    stream << "{ ";

    if (header.file != nullptr)
    {
        stream << header.file->path;
    }

    stream << ", includes: { ";

    for (const Header &include : header.includes)
    {
        stream << include << ", ";
    }

    stream << "}, importedHeaderUnits: { ";

    for (const HeaderUnit *unit : header.importedHeaderUnits)
    {
        stream << unit << ", ";
    }

    stream << "}, importedModules: { ";

    for (const Module *mod : header.importedModules)
    {
        stream << mod << ", ";
    }

    stream << "}, importedModulePartitions: { ";

    for (const ModulePartition *partition : header.importedModulePartitions)
    {
        stream << partition << ", ";
    }

    return stream << "} }";
}

[[nodiscard]] auto operator==(const Header &left, const Header &right) -> bool
{
    return left.file == right.file
        && left.includes == right.includes
        && left.importedHeaderUnits == right.importedHeaderUnits
        && left.importedModules == right.importedModules
        && left.importedModulePartitions == right.importedModulePartitions;
}
}

static const auto S = suite("Header", [] { // NOLINT(cert-err58-cpp)
    test("single header file", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path sourcePath = "source.cpp";
        const std::filesystem::path headerPath = "header.hpp";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *sourceFile = cache.add_source_file(sourcePath);
            ::abuild::HeaderFile *headerFile = cache.add_header_file(headerPath);
            assert_(sourceFile).not_to_be(nullptr);
            assert_(headerFile).not_to_be(nullptr);

            sourceFile->includes.emplace_back(::abuild::Header{.file = headerFile});
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *sourceFile = cache.exact_source_file(sourcePath);
        ::abuild::HeaderFile *headerFile = cache.exact_header_file(headerPath);
        assert_(sourceFile).not_to_be(nullptr);
        assert_(headerFile).not_to_be(nullptr);
        expect(sourceFile->includes).to_be(std::vector<::abuild::Header>{::abuild::Header{.file = headerFile}});
    });

    test("nested header file", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path sourcePath = "source.cpp";
        const std::filesystem::path headerPath1 = "header1.hpp";
        const std::filesystem::path headerPath2 = "header2.hpp";
        const std::filesystem::path headerPath3 = "header3.hpp";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *sourceFile = cache.add_source_file(sourcePath);
            ::abuild::HeaderFile *headerFile1 = cache.add_header_file(headerPath1);
            ::abuild::HeaderFile *headerFile2 = cache.add_header_file(headerPath2);
            ::abuild::HeaderFile *headerFile3 = cache.add_header_file(headerPath3);
            assert_(sourceFile).not_to_be(nullptr);
            assert_(headerFile1).not_to_be(nullptr);
            assert_(headerFile2).not_to_be(nullptr);
            assert_(headerFile3).not_to_be(nullptr);

            ::abuild::Header header3{.file = headerFile3};
            ::abuild::Header header2{.file = headerFile2};
            header2.includes.emplace_back(std::move(header3));
            ::abuild::Header header1{.file = headerFile1};
            header1.includes.emplace_back(std::move(header2));

            sourceFile->includes.emplace_back(std::move(header1));
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *sourceFile = cache.exact_source_file(sourcePath);
        ::abuild::HeaderFile *headerFile1 = cache.exact_header_file(headerPath1);
        ::abuild::HeaderFile *headerFile2 = cache.exact_header_file(headerPath2);
        ::abuild::HeaderFile *headerFile3 = cache.exact_header_file(headerPath3);
        assert_(sourceFile).not_to_be(nullptr);
        assert_(headerFile1).not_to_be(nullptr);
        assert_(headerFile2).not_to_be(nullptr);
        assert_(headerFile3).not_to_be(nullptr);

        ::abuild::Header header3{.file = headerFile3};
        ::abuild::Header header2{.file = headerFile2};
        header2.includes.emplace_back(std::move(header3));
        ::abuild::Header header1{.file = headerFile1};
        header1.includes.emplace_back(std::move(header2));

        const std::vector<::abuild::Header> expected{std::move(header1)};
        expect(sourceFile->includes).to_be(expected);
    });

    test("imported header unit", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path sourcePath = "source.cpp";
        const std::filesystem::path headerPath = "header.hpp";
        const std::filesystem::path headerUnitPath = "header_unit.hpp";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *sourceFile = cache.add_source_file(sourcePath);
            ::abuild::HeaderFile *headerFile = cache.add_header_file(headerPath);
            ::abuild::HeaderFile *headerUnitFile = cache.add_header_file(headerUnitPath);
            assert_(sourceFile).not_to_be(nullptr);
            assert_(headerFile).not_to_be(nullptr);
            assert_(headerUnitFile).not_to_be(nullptr);
            ::abuild::HeaderUnit *headerUnit = cache.add_header_unit(headerUnitFile);
            assert_(headerUnit).not_to_be(nullptr);

            ::abuild::Header header{.file = headerFile};
            header.importedHeaderUnits.push_back(headerUnit);

            sourceFile->includes.emplace_back(std::move(header));
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *sourceFile = cache.exact_source_file(sourcePath);
        assert_(sourceFile).not_to_be(nullptr);

        ::abuild::HeaderFile *headerFile = cache.exact_header_file(headerPath);
        assert_(headerFile).not_to_be(nullptr);

        ::abuild::HeaderFile *headerUnitFile = cache.exact_header_file(headerUnitPath);
        assert_(headerUnitFile).not_to_be(nullptr);

        ::abuild::HeaderUnit *headerUnit = cache.header_unit(headerUnitFile);
        assert_(headerUnit).not_to_be(nullptr);

        std::vector<::abuild::Header> expected{::abuild::Header{.file = headerFile}};
        expected[0].importedHeaderUnits.push_back(headerUnit);

        expect(sourceFile->includes).to_be(expected);
    });

    test("imported module", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path sourcePath = "source.cpp";
        const std::filesystem::path headerPath = "header.hpp";
        const std::filesystem::path moduleSourcePath = "module.cpp";
        const std::string moduleName = "my_module";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *sourceFile = cache.add_source_file(sourcePath);
            ::abuild::HeaderFile *headerFile = cache.add_header_file(headerPath);
            ::abuild::SourceFile *moduleSourceFile = cache.add_source_file(moduleSourcePath);
            ::abuild::Module *mod = cache.add_module(moduleName);

            assert_(sourceFile).not_to_be(nullptr);
            assert_(headerFile).not_to_be(nullptr);
            assert_(moduleSourceFile).not_to_be(nullptr);
            assert_(mod).not_to_be(nullptr);

            mod->sourceFile = moduleSourceFile;

            ::abuild::Header header{.file = headerFile};
            header.importedModules.push_back(mod);

            sourceFile->includes.emplace_back(std::move(header));
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *sourceFile = cache.exact_source_file(sourcePath);
        assert_(sourceFile).not_to_be(nullptr);

        ::abuild::HeaderFile *headerFile = cache.exact_header_file(headerPath);
        assert_(headerFile).not_to_be(nullptr);

        ::abuild::Module *mod = cache.module_(moduleName);
        assert_(mod).not_to_be(nullptr);

        std::vector<::abuild::Header> expected{::abuild::Header{.file = headerFile}};
        expected[0].importedModules.push_back(mod);

        expect(sourceFile->includes).to_be(expected);
    });

    test("imported module partition", [] {
        const ::abuild::TestFile testFile{"./abuild.cache_test.yaml"};
        const std::filesystem::path sourcePath = "source.cpp";
        const std::filesystem::path headerPath = "header.hpp";
        const std::filesystem::path moduleSourcePath = "module.cpp";
        const std::filesystem::path modulePartitionSourcePath = "module.cpp";
        const std::string moduleName = "my_module";
        const std::string modulePartitionName = "my_partition";

        {
            ::abuild::Cache cache{testFile.path()};
            ::abuild::SourceFile *sourceFile = cache.add_source_file(sourcePath);
            ::abuild::HeaderFile *headerFile = cache.add_header_file(headerPath);
            ::abuild::SourceFile *moduleSourceFile = cache.add_source_file(moduleSourcePath);
            ::abuild::SourceFile *modulePartitionSourceFile = cache.add_source_file(modulePartitionSourcePath);
            ::abuild::Module *mod = cache.add_module(moduleName);
            ::abuild::ModulePartition *partition = cache.add_module_partition(modulePartitionName);

            assert_(sourceFile).not_to_be(nullptr);
            assert_(headerFile).not_to_be(nullptr);
            assert_(moduleSourceFile).not_to_be(nullptr);
            assert_(mod).not_to_be(nullptr);
            assert_(modulePartitionSourceFile).not_to_be(nullptr);
            assert_(partition).not_to_be(nullptr);

            mod->sourceFile = moduleSourceFile;
            partition->mod = mod;
            partition->sourceFile = modulePartitionSourceFile;
            mod->partitions.push_back(partition);

            ::abuild::Header header{.file = headerFile};
            header.importedModulePartitions.push_back(partition);

            sourceFile->includes.emplace_back(std::move(header));
        }

        ::abuild::Cache cache{testFile.path()};
        ::abuild::SourceFile *sourceFile = cache.exact_source_file(sourcePath);
        assert_(sourceFile).not_to_be(nullptr);

        ::abuild::HeaderFile *headerFile = cache.exact_header_file(headerPath);
        assert_(headerFile).not_to_be(nullptr);

        ::abuild::Module *mod = cache.module_(moduleName);
        assert_(mod).not_to_be(nullptr);
        assert_(mod->partitions.size()).to_be(1U);
        assert_(mod->partitions[0]).not_to_be(nullptr);

        std::vector<::abuild::Header> expected{::abuild::Header{.file = headerFile}};
        expected[0].importedModulePartitions.push_back(mod->partitions[0]);

        expect(sourceFile->includes).to_be(expected);
    });
});
