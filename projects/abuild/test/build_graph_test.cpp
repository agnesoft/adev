import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

[[nodiscard]] auto findProject(const std::vector<std::unique_ptr<abuild::Project>> &projects, const std::string &name) -> abuild::Project *
{
    auto it = std::find_if(projects.begin(),
                           projects.end(),
                           [&](const std::unique_ptr<abuild::Project> &project) {
                               return project->name() == name;
                           });

    return it != projects.end() ? it->get() : nullptr;
};

static const auto testSuite = suite("abuild::BuildGraph", [] {
    test("no sources", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"header.hpp", ""}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.buildTasks().size()).toBe(0u);
    });

    test("executable", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", ""}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.projects().size()).toBe(1u);

        const abuild::BuildTask *linkTask = cache.buildTask(cache.projects()[0].get());
        const abuild::BuildTask *compileTask = cache.buildTask(cache.sources()[0].get());

        assert_(linkTask != nullptr).toBe(true);
        assert_(compileTask != nullptr).toBe(true);

        const abuild::LinkExecutableTask *link = &std::get<abuild::LinkExecutableTask>(*linkTask);
        const abuild::CompileSourceTask *compile = &std::get<abuild::CompileSourceTask>(*compileTask);

        expect(link->project).toBe(cache.projects()[0].get());
        expect(compile->source).toBe(cache.sources()[0].get());

        assert_(link->inputTasks.size()).toBe(1u);
        expect(*link->inputTasks.begin()).toBe(compileTask);
    });

    test("library", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"source.cpp", ""}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        assert_(cache.sources().size()).toBe(1u);
        assert_(cache.projects().size()).toBe(1u);

        const abuild::BuildTask *linkTask = cache.buildTask(cache.projects()[0].get());
        const abuild::BuildTask *compileTask = cache.buildTask(cache.sources()[0].get());

        assert_(linkTask != nullptr).toBe(true);
        assert_(compileTask != nullptr).toBe(true);

        const abuild::LinkLibraryTask *link = &std::get<abuild::LinkLibraryTask>(*linkTask);
        const abuild::CompileSourceTask *compile = &std::get<abuild::CompileSourceTask>(*compileTask);

        expect(link->project).toBe(cache.projects()[0].get());
        expect(compile->source).toBe(cache.sources()[0].get());

        assert_(link->inputTasks.size()).toBe(1u);
        expect(*link->inputTasks.begin()).toBe(compileTask);
    });

    test("module library", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"mymodule.cpp", "module mymodule;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        assert_(cache.modules().size()).toBe(1u);
        assert_(cache.sources().size()).toBe(1u);

        const abuild::BuildTask *linkTask = cache.buildTask(cache.modules()[0].get());
        const abuild::BuildTask *compileTask = cache.buildTask(cache.sources()[0].get());

        assert_(linkTask != nullptr).toBe(true);
        assert_(compileTask != nullptr).toBe(true);

        const abuild::LinkModuleLibraryTask *link = &std::get<abuild::LinkModuleLibraryTask>(*linkTask);
        const abuild::CompileModuleInterfaceTask *compile = &std::get<abuild::CompileModuleInterfaceTask>(*compileTask);

        expect(link->mod).toBe(cache.modules()[0].get());
        expect(compile->source).toBe(cache.sources()[0].get());

        assert_(link->inputTasks.size()).toBe(1u);
        expect(*link->inputTasks.begin()).toBe(compileTask);
    });

    test("module partition", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"mypartition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        const abuild::BuildTask *compilePartitionTask = cache.buildTask(cache.source("mypartition.cpp"));

        assert_(compilePartitionTask != nullptr).toBe(true);

        const abuild::CompileModulePartitionTask *compilePartition = &std::get<abuild::CompileModulePartitionTask>(*compilePartitionTask);

        expect(compilePartition->source).toBe(cache.source("mypartition.cpp"));
    });

    test("module with partition", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"mymodule.cpp", "module mymodule; import : mypartition;"},
                                            {"mypartition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        assert_(cache.modules().size()).toBe(1u);

        abuild::BuildTask *linkTask = cache.buildTask(cache.modules()[0].get());
        abuild::BuildTask *compileModuleTask = cache.buildTask(cache.source("mymodule.cpp"));
        abuild::BuildTask *compilePartitionTask = cache.buildTask(cache.source("mypartition.cpp"));

        assert_(linkTask != nullptr).toBe(true);
        assert_(compilePartitionTask != nullptr).toBe(true);

        const auto *compile = &std::get<abuild::CompileModuleInterfaceTask>(*compileModuleTask);

        assert_(compile->inputTasks.size()).toBe(1u);
        expect(*compile->inputTasks.begin()).toBe(compilePartitionTask);

        expect(std::get<abuild::LinkModuleLibraryTask>(*linkTask).inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                compileModuleTask,
                compilePartitionTask});
    });

    test("include header that links in library", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", "#include <myheader.hpp>\n#include <headeronly.hpp>"},
                                            {"mylib/mysource.cpp", ""},
                                            {"mylib/myheader.hpp", ""},
                                            {"headeronly/headeronly.hpp", ""}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        const abuild::Project *exe = findProject(cache.projects(), "abuild_build_graph_test");
        const abuild::Project *lib = findProject(cache.projects(), "mylib");
        const abuild::Project *headerLib = findProject(cache.projects(), "headeronly");

        assert_(exe != nullptr).toBe(true);
        assert_(lib != nullptr).toBe(true);
        assert_(headerLib != nullptr).toBe(true);

        abuild::BuildTask *compileTask = cache.buildTask(cache.source("main.cpp"));
        abuild::BuildTask *linkExecutableTask = cache.buildTask(exe);
        abuild::BuildTask *linkLibraryTask = cache.buildTask(lib);
        abuild::BuildTask *linkHeaderOnlyLibraryTask = cache.buildTask(headerLib);

        assert_(compileTask != nullptr).toBe(true);
        assert_(linkExecutableTask != nullptr).toBe(true);
        assert_(linkLibraryTask != nullptr).toBe(true);
        expect(linkHeaderOnlyLibraryTask).toBe(nullptr);

        expect(std::get<abuild::LinkExecutableTask>(*linkExecutableTask).inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                compileTask,
                linkLibraryTask});
        expect(std::get<abuild::CompileSourceTask>(*compileTask).includePaths)
            .toBe(std::unordered_set<std::filesystem::path, abuild::PathHash>{
                testProject.projectRoot() / "mylib",
                testProject.projectRoot() / "headeronly"});
    });

    test("include header that links in library via another header", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", "#include <headeronly.hpp>"},
                                            {"mylib/mysource.cpp", ""},
                                            {"mylib/myheader.hpp", ""},
                                            {"headeronly/headeronly.hpp", "#include <myheader.hpp>"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        const abuild::Project *exe = findProject(cache.projects(), "abuild_build_graph_test");
        const abuild::Project *lib = findProject(cache.projects(), "mylib");
        const abuild::Project *headerLib = findProject(cache.projects(), "headeronly");

        assert_(exe != nullptr).toBe(true);
        assert_(lib != nullptr).toBe(true);
        assert_(headerLib != nullptr).toBe(true);

        abuild::BuildTask *compileTask = cache.buildTask(cache.source("main.cpp"));
        abuild::BuildTask *linkExecutableTask = cache.buildTask(exe);
        abuild::BuildTask *linkLibraryTask = cache.buildTask(lib);
        abuild::BuildTask *linkHeaderOnlyLibraryTask = cache.buildTask(headerLib);

        assert_(compileTask != nullptr).toBe(true);
        assert_(linkExecutableTask != nullptr).toBe(true);
        assert_(linkLibraryTask != nullptr).toBe(true);
        expect(linkHeaderOnlyLibraryTask).toBe(nullptr);

        expect(std::get<abuild::LinkExecutableTask>(*linkExecutableTask).inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                compileTask,
                linkLibraryTask});

        expect(std::get<abuild::CompileSourceTask>(*compileTask).includePaths)
            .toBe(std::unordered_set<std::filesystem::path, abuild::PathHash>{
                testProject.projectRoot() / "mylib",
                testProject.projectRoot() / "headeronly"});
    });

    test("import module", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", "import mymodule;"},
                                            {"mymodule.cpp", "module mymodule;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        assert_(cache.projects().size()).toBe(1u);
        assert_(cache.modules().size()).toBe(1u);

        abuild::BuildTask *linkExecutableTask = cache.buildTask(cache.projects()[0].get());
        abuild::BuildTask *linkModuleTask = cache.buildTask(cache.modules()[0].get());
        abuild::BuildTask *compileTask = cache.buildTask(cache.source("main.cpp"));
        abuild::BuildTask *compileModuleTask = cache.buildTask(cache.source("mymodule.cpp"));

        assert_(linkExecutableTask != nullptr).toBe(true);
        assert_(linkModuleTask != nullptr).toBe(true);
        assert_(compileTask != nullptr).toBe(true);
        assert_(compileModuleTask != nullptr).toBe(true);

        const auto *compile = &std::get<abuild::CompileSourceTask>(*compileTask);
        const auto *link = &std::get<abuild::LinkExecutableTask>(*linkExecutableTask);

        expect(compile->inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                compileModuleTask});

        expect(link->inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                compileTask,
                linkModuleTask});
    });

    test("import module via header", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", "#include \"header.hpp\""},
                                            {"header.hpp", "import mymodule;"},
                                            {"mymodule.cpp", "module mymodule;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        assert_(cache.projects().size()).toBe(1u);
        assert_(cache.modules().size()).toBe(1u);

        abuild::BuildTask *linkExecutableTask = cache.buildTask(cache.projects()[0].get());
        abuild::BuildTask *linkModuleTask = cache.buildTask(cache.modules()[0].get());
        abuild::BuildTask *compileTask = cache.buildTask(cache.source("main.cpp"));
        abuild::BuildTask *compileModuleTask = cache.buildTask(cache.source("mymodule.cpp"));

        assert_(linkExecutableTask != nullptr).toBe(true);
        assert_(linkModuleTask != nullptr).toBe(true);
        assert_(compileTask != nullptr).toBe(true);
        assert_(compileModuleTask != nullptr).toBe(true);

        const auto *compile = &std::get<abuild::CompileSourceTask>(*compileTask);
        const auto *link = &std::get<abuild::LinkExecutableTask>(*linkExecutableTask);

        expect(compile->inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                compileModuleTask});

        expect(link->inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                compileTask,
                linkModuleTask});
    });

    test("import header", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", "import <mylib.hpp>;"},
                                            {"mylib/mylib.hpp", "import <otherlib.hpp>"},
                                            {"otherlib/otherlib.cpp", "import \"otherlib.hpp\";"},
                                            {"otherlib/otherlib.hpp", ""}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        const abuild::Project *exe = findProject(cache.projects(), "abuild_build_graph_test");
        const abuild::Project *mylib = findProject(cache.projects(), "mylib");
        const abuild::Project *otherlib = findProject(cache.projects(), "otherlib");

        assert_(exe != nullptr).toBe(true);
        assert_(mylib != nullptr).toBe(true);
        assert_(otherlib != nullptr).toBe(true);

        abuild::BuildTask *compileMainTask = cache.buildTask(cache.source("main.cpp"));
        abuild::BuildTask *linkExecutableTask = cache.buildTask(exe);
        abuild::BuildTask *compileMyLibHppTask = cache.buildTask(cache.header("mylib.hpp"));
        abuild::BuildTask *linkMyLibTask = cache.buildTask(mylib);
        abuild::BuildTask *compileOtherLibHppTask = cache.buildTask(cache.header("otherlib.hpp"));
        abuild::BuildTask *compileOtherLibTask = cache.buildTask(cache.source("otherlib.cpp"));
        abuild::BuildTask *linkOtherLibTask = cache.buildTask(otherlib);

        assert_(compileMainTask != nullptr).toBe(true);
        assert_(linkExecutableTask != nullptr).toBe(true);
        assert_(compileMyLibHppTask != nullptr).toBe(true);
        assert_(linkMyLibTask != nullptr).toBe(true);
        assert_(compileOtherLibHppTask != nullptr).toBe(true);
        assert_(compileOtherLibTask != nullptr).toBe(true);
        assert_(linkOtherLibTask != nullptr).toBe(true);

        const auto *compileMain = &std::get<abuild::CompileSourceTask>(*compileMainTask);
        const auto *linkExe = &std::get<abuild::LinkExecutableTask>(*linkExecutableTask);
        const auto *compileMyLibHeaderUnit = &std::get<abuild::CompileHeaderUnitTask>(*compileMyLibHppTask);
        const auto *linkMyLib = &std::get<abuild::LinkLibraryTask>(*linkMyLibTask);
        const auto *compileOtherLib = &std::get<abuild::CompileSourceTask>(*compileOtherLibTask);
        const auto *linkOtherLib = &std::get<abuild::LinkLibraryTask>(*linkOtherLibTask);

        expect(compileMain->inputTasks).toBe(std::unordered_set<abuild::BuildTask *>{compileMyLibHppTask});
        expect(linkExe->inputTasks).toBe(std::unordered_set<abuild::BuildTask *>{compileMainTask, linkMyLibTask});
        expect(compileMyLibHeaderUnit->inputTasks).toBe(std::unordered_set<abuild::BuildTask *>{compileOtherLibHppTask});
        expect(linkMyLib->inputTasks).toBe(std::unordered_set<abuild::BuildTask *>{linkOtherLibTask, compileMyLibHppTask});
        expect(compileOtherLib->inputTasks).toBe(std::unordered_set<abuild::BuildTask *>{compileOtherLibHppTask});
        expect(linkOtherLib->inputTasks).toBe(std::unordered_set<abuild::BuildTask *>{compileOtherLibHppTask, compileOtherLibTask});
    });

    test("include source", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"mymodule.cpp", "module mymodule;\n#include \"mypartition.cpp\""},
                                            {"mypartition.cpp", "module mymodule : mypartition;\nimport othermodule;"},
                                            {"othermodule/othermodule.cpp", "module othermodule;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        abuild::BuildTask *compileTask = cache.buildTask(cache.source("mymodule.cpp"));
        abuild::BuildTask *otherCompileTask = cache.buildTask(cache.source("othermodule.cpp"));

        assert_(compileTask != nullptr).toBe(true);
        assert_(otherCompileTask != nullptr).toBe(true);

        const auto *compile = &std::get<abuild::CompileModuleInterfaceTask>(*compileTask);

        expect(compile->inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                otherCompileTask});
    });

    test("include external source", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"mymodule.cpp", "module mymodule;\n#include <mypartition.cpp>"},
                                            {"someproject/mypartition.cpp", "module mymodule : mypartition;\nimport othermodule;"},
                                            {"othermodule/othermodule.cpp", "module othermodule;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        abuild::BuildTask *compileTask = cache.buildTask(cache.source("mymodule.cpp"));
        abuild::BuildTask *otherCompileTask = cache.buildTask(cache.source("othermodule.cpp"));

        assert_(compileTask != nullptr).toBe(true);
        assert_(otherCompileTask != nullptr).toBe(true);

        const auto *compile = &std::get<abuild::CompileModuleInterfaceTask>(*compileTask);

        expect(compile->inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                otherCompileTask});
    });

    test("include source from another project", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", "\n#include \"mysource.cpp\""},
                                            {"mylib/mysource.cpp", ""}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        const abuild::Project *exe = findProject(cache.projects(), "abuild_build_graph_test");
        const abuild::Project *lib = findProject(cache.projects(), "mylib");

        assert_(exe != nullptr).toBe(true);
        assert_(lib != nullptr).toBe(true);

        abuild::BuildTask *compileTask = cache.buildTask(cache.source("main.cpp"));
        abuild::BuildTask *linkTask = cache.buildTask(exe);
        abuild::BuildTask *linkLib = cache.buildTask(lib);

        assert_(compileTask != nullptr).toBe(true);
        assert_(linkTask != nullptr).toBe(true);
        assert_(linkLib != nullptr).toBe(true);

        const auto *compile = &std::get<abuild::CompileSourceTask>(*compileTask);
        const auto *link = &std::get<abuild::LinkExecutableTask>(*linkTask);

        expect(compile->inputTasks).toBe(std::unordered_set<abuild::BuildTask *>{});
        expect(compile->includePaths).toBe(std::unordered_set<std::filesystem::path, abuild::PathHash>{testProject.projectRoot() / "mylib"});
        expect(link->inputTasks).toBe(std::unordered_set<abuild::BuildTask *>{compileTask});
    });

    test("self include", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", "\n#include \"myheader.hpp\""},
                                            {"mylib/myheader.hpp", "#include \"myheader.hpp\""}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        abuild::BuildTask *compileTask = cache.buildTask(cache.source("main.cpp"));

        expect(compileTask != nullptr).toBe(true);
    });

    test("circular include", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", "\n#include \"myheader.hpp\""},
                                            {"mylib/myheader.hpp", "#include \"otherheader.hpp\""},
                                            {"mylib/otherheader.hpp", "#include \"myheader.hpp\""}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        abuild::BuildTask *compileTask = cache.buildTask(cache.source("main.cpp"));

        expect(compileTask != nullptr).toBe(true);
    });

    test("import STL", [] {
        TestProjectWithContent testProject{"abuild_build_graph_test",
                                           {{"main.cpp", "import <vector>;"}}};

        abuild::BuildCache cache{testProject.projectRoot()};
        abuild::ProjectScanner{cache};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        abuild::BuildTask *compileTask = cache.buildTask(cache.source("main.cpp"));
        abuild::BuildTask *compileSTLHeaderUnitTask = cache.buildTask("vector");

        assert_(compileTask != nullptr).toBe(true);
        assert_(compileSTLHeaderUnitTask != nullptr).toBe(true);

        const auto *compile = &std::get<abuild::CompileSourceTask>(*compileTask);

        expect(compile->inputTasks)
            .toBe(std::unordered_set<abuild::BuildTask *>{
                compileSTLHeaderUnitTask});
    });
});
