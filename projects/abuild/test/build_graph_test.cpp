import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::BuildGraph", [] {
    test("project with no sources", [] {
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

    test("compile & link single executable", [] {
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
        expect(link->inputTasks[0]).toBe(compileTask);
    });

    test("compile & link single library", [] {
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
        expect(link->inputTasks[0]).toBe(compileTask);
    });

    test("compile & link module library", [] {
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
        expect(link->inputTasks[0]).toBe(compileTask);
    });

    test("compile module partition", [] {
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
});
