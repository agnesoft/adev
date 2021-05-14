import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Module", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Module>).toBe(true);
        expect(std::is_copy_constructible_v<abuild::Module>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Module>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Module>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<abuild::Module>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::Module>).toBe(true);
    });

    test("type traits (Partition)", [] {
        expect(std::is_default_constructible_v<abuild::Module::Partition>).toBe(true);
        expect(std::is_copy_constructible_v<abuild::Module::Partition>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Module::Partition>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Module::Partition>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<abuild::Module::Partition>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::Module::Partition>).toBe(true);
    });

    test("add module interface", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"mymodule.cpp"}};

        abuild::BuildCache cache;
        abuild::Project project{"build_test_project_scanner"};
        abuild::Source source{testProject.projectRoot() / "mymodule.cpp", &project};

        cache.addModuleInterface("mymodule", &source);

        assert_(cache.modules().size()).toBe(1u);
        expect(cache.modules()[0]->name).toBe("mymodule");
        expect(cache.modules()[0]->source).toBe(&source);
    });

    test("add module partition", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"mymodule.cpp",
                                 "mypartition.cpp"}};

        abuild::BuildCache cache;
        abuild::Project project{"build_test_project_scanner"};
        abuild::Source source1{testProject.projectRoot() / "mymodule.cpp", &project};
        abuild::Source source2{testProject.projectRoot() / "mypartition.cpp", &project};

        cache.addModuleInterface("mymodule", &source1);
        cache.addModulePartition("mymodule", "mypartition", &source2);

        assert_(cache.modules().size()).toBe(1u);
        assert_(cache.modules()[0]->partitions.size()).toBe(1u);

        expect(cache.modules()[0]->partitions[0].name).toBe("mypartition");
        expect(cache.modules()[0]->partitions[0].source).toBe(&source2);
    });

    test("add module partitions", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"mymodule.cpp",
                                 "mypartition.cpp",
                                 "mypartition2.cpp"}};

        abuild::BuildCache cache;
        abuild::Project project{"build_test_project_scanner"};
        abuild::Source source1{testProject.projectRoot() / "mymodule.cpp", &project};
        abuild::Source source2{testProject.projectRoot() / "mypartition.cpp", &project};
        abuild::Source source3{testProject.projectRoot() / "mypartition2.cpp", &project};

        cache.addModuleInterface("mymodule", &source1);
        cache.addModulePartition("mymodule", "mypartition", &source2);
        cache.addModulePartition("mymodule", "mypartition2", &source3);

        assert_(cache.modules().size()).toBe(1u);
        assert_(cache.modules()[0]->partitions.size()).toBe(2u);

        expect(cache.modules()[0]->partitions[0].name).toBe("mypartition");
        expect(cache.modules()[0]->partitions[0].source).toBe(&source2);

        expect(cache.modules()[0]->partitions[1].name).toBe("mypartition2");
        expect(cache.modules()[0]->partitions[1].source).toBe(&source3);
    });

    test("add module partition before interface", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"mymodule.cpp",
                                 "mypartition.cpp"}};

        abuild::BuildCache cache;
        abuild::Project project{"build_test_project_scanner"};
        abuild::Source source2{testProject.projectRoot() / "mypartition.cpp", &project};

        cache.addModulePartition("mymodule", "mypartition", &source2);

        assert_(cache.modules().size()).toBe(1u);

        expect(cache.modules()[0]->name).toBe("mymodule");
        expect(cache.modules()[0]->source).toBe(nullptr);

        assert_(cache.modules()[0]->partitions.size()).toBe(1u);

        expect(cache.modules()[0]->partitions[0].name).toBe("mypartition");
        expect(cache.modules()[0]->partitions[0].source).toBe(&source2);
    });
});
