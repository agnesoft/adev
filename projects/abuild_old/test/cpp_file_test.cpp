import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

namespace std
{
auto operator<<(std::ostream &stream, const std::pair<std::string, abuild::File *> &value) -> std::ostream &
{
    stream << '{' << value.first << ": " << (value.second ? value.second->path().string() : "nullptr") << '}';
    return stream;
}
}

static const auto testSuite = suite("abuild::CppFile", [] {
    test("imports external", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "vector",
                                 "string"}};

        abuild::CppFile file{testProject.projectRoot() / "main.cpp"};

        expect(file.importsExternal()).toBe(std::unordered_map<std::string, abuild::File *>{});

        abuild::File file1{testProject.projectRoot() / "vector"};
        abuild::File file2{testProject.projectRoot() / "string"};

        file.addImportExternal("vector", &file1);
        file.addImportExternal("string", &file2);

        expect(std::as_const(file.importsExternal()))
            .toBe(std::unordered_map<std::string, abuild::File *>{
                {"vector", &file1},
                {"string", &file2}});
    });

    test("imports local", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "include/header1.hpp",
                                 "include/header2.hpp"}};

        abuild::CppFile file{testProject.projectRoot() / "main.cpp"};

        expect(file.importsLocal()).toBe(std::unordered_map<std::string, abuild::File *>{});

        abuild::File file1{testProject.projectRoot() / "include" / "header1.hpp"};
        abuild::File file2{testProject.projectRoot() / "include" / "header2.hpp"};

        file.addImportLocal("include/header1.hpp", &file1);
        file.addImportLocal("include/header2.hpp", &file2);

        expect(std::as_const(file.importsLocal()))
            .toBe(std::unordered_map<std::string, abuild::File *>{
                {"include/header1.hpp", &file1},
                {"include/header2.hpp", &file2}});
    });

    test("imports module partitions", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"my_module.cpp",
                                 "my_partition.cpp",
                                 "my_other_partition.cpp"}};

        abuild::CppFile file{testProject.projectRoot() / "my_module.cpp"};

        expect(file.importModulePartitions()).toBe(std::unordered_map<std::string, abuild::File *>{});

        abuild::File file1{testProject.projectRoot() / "my_partition.cpp"};
        abuild::File file2{testProject.projectRoot() / "my_other_partition.cpp"};

        file.addImportModulePartition("my_partition", &file1);
        file.addImportModulePartition("my_other_partition", &file2);

        expect(std::as_const(file.importModulePartitions()))
            .toBe(std::unordered_map<std::string, abuild::File *>{
                {"my_partition", &file1},
                {"my_other_partition", &file2}});
    });

    test("imports modules", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"my_module.cpp",
                                 "my_other_module.cpp",
                                 "yet_anothermodule.cpp"}};

        abuild::CppFile file{testProject.projectRoot() / "my_module.cpp"};

        expect(file.importModules()).toBe(std::unordered_map<std::string, abuild::File *>{});

        abuild::File file1{testProject.projectRoot() / "my_other_module.cpp"};
        abuild::File file2{testProject.projectRoot() / "yet_anothermodule.cpp"};

        file.addImportModule("my_other_module", &file1);
        file.addImportModule("yet_anothermodule", &file2);

        expect(std::as_const(file.importModules()))
            .toBe(std::unordered_map<std::string, abuild::File *>{
                {"my_other_module", &file1},
                {"yet_anothermodule", &file2}});
    });

    test("includes external", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "vector",
                                 "string"}};

        abuild::CppFile file{testProject.projectRoot() / "main.cpp"};

        expect(file.includesExternal()).toBe(std::unordered_map<std::string, abuild::File *>{});

        abuild::File file1{testProject.projectRoot() / "vector"};
        abuild::File file2{testProject.projectRoot() / "string"};

        file.addIncludeExternal("vector", &file1);
        file.addIncludeExternal("string", &file2);

        expect(std::as_const(file.includesExternal()))
            .toBe(std::unordered_map<std::string, abuild::File *>{
                {"vector", &file1},
                {"string", &file2}});
    });

    test("includes local", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp",
                                 "include/header1.hpp",
                                 "include/header2.hpp"}};

        abuild::CppFile file{testProject.projectRoot() / "main.cpp"};

        expect(file.includesLocal()).toBe(std::unordered_map<std::string, abuild::File *>{});

        abuild::File file1{testProject.projectRoot() / "include" / "header1.hpp"};
        abuild::File file2{testProject.projectRoot() / "include" / "header2.hpp"};

        file.addIncludeLocal("include/header1.hpp", &file1);
        file.addIncludeLocal("include/header2.hpp", &file2);

        expect(std::as_const(file.includesLocal()))
            .toBe(std::unordered_map<std::string, abuild::File *>{
                {"include/header1.hpp", &file1},
                {"include/header2.hpp", &file2}});
    });

    test("module", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        abuild::CppFile file{testProject.projectRoot() / "main.cpp"};

        expect(file.isModule()).toBe(false);
        expect(file.isModulePartition()).toBe(false);
        expect(file.moduleName()).toBe(std::string{});

        file.setModuleName("mymodule");

        expect(file.isModule()).toBe(true);
        expect(file.isModulePartition()).toBe(false);
        expect(file.moduleName()).toBe(std::string{"mymodule"});
    });

    test("module partition", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        abuild::CppFile file{testProject.projectRoot() / "main.cpp"};

        expect(file.isModule()).toBe(false);
        expect(file.isModulePartition()).toBe(false);
        expect(file.moduleName()).toBe(std::string{});
        expect(file.modulePartition()).toBe(std::string{});

        file.setModulePartition("mymodule", "mypartition");

        expect(file.isModule()).toBe(false);
        expect(file.isModulePartition()).toBe(true);
        expect(file.moduleName()).toBe(std::string{"mymodule"});
        expect(file.modulePartition()).toBe(std::string{"mypartition"});
    });
});
