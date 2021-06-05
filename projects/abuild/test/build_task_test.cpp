import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::BuildTask", [] {
    test("add build task", [] {
        abuild::BuildCache cache;

        abuild::Project project{"myproject"};
        cache.addBuildTask(&project, abuild::LinkExecutableTask{.project = &project});

        expect(cache.buildTasks().size()).toBe(1u);
    });

    test("lookup build task", [] {
        abuild::BuildCache cache;

        abuild::Project project1{"myproject1"};
        abuild::Project project2{"myproject2"};
        abuild::Project project3{"myproject3"};
        abuild::Project project4{"myproject4"};
        cache.addBuildTask(&project1, abuild::LinkExecutableTask{.project = &project1});
        cache.addBuildTask(&project2, abuild::LinkExecutableTask{.project = &project2});
        cache.addBuildTask(&project3, abuild::LinkExecutableTask{.project = &project3});

        expect(cache.buildTask(&project4)).toBe(nullptr);
        expect(cache.buildTask(&project2) != nullptr).toBe(true);
        expect(std::get<abuild::LinkExecutableTask>(*cache.buildTask(&project2)).project).toBe(&project2);
    });

    test("lookup build task by name", [] {
        abuild::BuildCache cache;

        cache.addBuildTask("string", abuild::CompileSTLHeaderUnitTask{.name = "string"});
        cache.addBuildTask("vector", abuild::CompileSTLHeaderUnitTask{.name = "vector"});
        cache.addBuildTask("map", abuild::CompileSTLHeaderUnitTask{.name = "map"});

        expect(std::get<abuild::CompileSTLHeaderUnitTask>(*cache.buildTask("vector")).name).toBe("vector");
    });
});
