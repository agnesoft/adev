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
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"header.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        assert_(cache.projects().size()).toBe(1u);
        expect(cache.buildTasks().size()).toBe(0u);
    });

    test("executable", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
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
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"source.cpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
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
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "module mymodule;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
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
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mypartition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        const abuild::BuildTask *compilePartitionTask = cache.buildTask(cache.source("mypartition.cpp"));

        assert_(compilePartitionTask != nullptr).toBe(true);

        const abuild::CompileModulePartitionTask *compilePartition = &std::get<abuild::CompileModulePartitionTask>(*compilePartitionTask);

        expect(compilePartition->source).toBe(cache.source("mypartition.cpp"));
    });

    test("module with partition", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"mymodule.cpp", "module mymodule; import : mypartition;"},
                                            {"mypartition.cpp", "module mymodule : mypartition;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
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
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include <myheader.hpp>\n#include <headeronly.hpp>"},
                                            {"mylib/mysource.cpp", ""},
                                            {"mylib/myheader.hpp", ""},
                                            {"headeronly/headeronly.hpp", ""}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        const abuild::Project *exe = findProject(cache.projects(), "build_test_project_scanner");
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
    });

    test("include header that links in library via another header", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include <headeronly.hpp>"},
                                            {"mylib/mysource.cpp", ""},
                                            {"mylib/myheader.hpp", ""},
                                            {"headeronly/headeronly.hpp", "#include <myheader.hpp>"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
        abuild::CodeScanner{cache};
        abuild::DependencyScanner{cache};
        abuild::BuildGraph{cache};

        const abuild::Project *exe = findProject(cache.projects(), "build_test_project_scanner");
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
    });

    test("import module", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "import mymodule;"},
                                            {"mymodule.cpp", "module mymodule;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
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
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "#include \"header.hpp\""},
                                            {"header.hpp", "import mymodule;"},
                                            {"mymodule.cpp", "module mymodule;"}}};

        abuild::BuildCache cache;
        abuild::ProjectScanner{cache, testProject.projectRoot()};
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
});
