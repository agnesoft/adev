import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Dependency", [] {
    test("IncludeExternalHeaderDependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "rapidjson/include/rapidjson.h"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Header header{testProject.projectRoot() / "rapidjson" / "include" / "rapidjson.h", nullptr};

        source.addDependency(abuild::IncludeExternalHeaderDependency{.name = "rapidjson.h", .header = &header});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::IncludeExternalHeaderDependency>(source.dependencies()[0]).name).toBe("rapidjson.h");
        expect(std::get<abuild::IncludeExternalHeaderDependency>(source.dependencies()[0]).header).toBe(&header);
    });

    test("IncludeLocalHeaderDependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "myheader.h"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Header header{testProject.projectRoot() / "myheader.h", nullptr};

        source.addDependency(abuild::IncludeLocalHeaderDependency{.name = "myheader.h", .header = &header});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::IncludeLocalHeaderDependency>(source.dependencies()[0]).name).toBe("myheader.h");
        expect(std::get<abuild::IncludeLocalHeaderDependency>(source.dependencies()[0]).header).toBe(&header);
    });

    test("IncludeLocalSourceDependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "source1.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Source otherSource{testProject.projectRoot() / "source1.cpp", nullptr};

        source.addDependency(abuild::IncludeLocalSourceDependency{.name = "source1.cpp", .source = &otherSource});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::IncludeLocalSourceDependency>(source.dependencies()[0]).name).toBe("source1.cpp");
        expect(std::get<abuild::IncludeLocalSourceDependency>(source.dependencies()[0]).source).toBe(&otherSource);
    });

    test("IncludeSTLHeaderDependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};

        source.addDependency(abuild::IncludeSTLHeaderDependency{.name = "vector"});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::IncludeSTLHeaderDependency>(source.dependencies()[0]).name).toBe("vector");
    });

    test("ImportExternalHeaderDependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "rapidjson/include/rapidjson.h"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Header header{testProject.projectRoot() / "rapidjson" / "include" / "rapidjson.h", nullptr};

        source.addDependency(abuild::ImportExternalHeaderDependency{"rapidjson.h", &header});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportExternalHeaderDependency>(source.dependencies()[0]).name).toBe("rapidjson.h");
        expect(std::get<abuild::ImportExternalHeaderDependency>(source.dependencies()[0]).header).toBe(&header);
    });

    test("ImportLocalHeaderDependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "myheader.h"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Header header{testProject.projectRoot() / "myheader.h", nullptr};

        source.addDependency(abuild::ImportLocalHeaderDependency{.name = "myheader.h", .header = &header});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportLocalHeaderDependency>(source.dependencies()[0]).name).toBe("myheader.h");
        expect(std::get<abuild::ImportLocalHeaderDependency>(source.dependencies()[0]).header).toBe(&header);
    });

    test("ImportModuleDependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "mymodule.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Source modSource{testProject.projectRoot() / "mymodule.cpp", nullptr};
        abuild::Module mod{"mymodule", &modSource};

        source.addDependency(abuild::ImportModuleDependency{.name = "mymodule", .mod = &mod});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportModuleDependency>(source.dependencies()[0]).name).toBe("mymodule");
        expect(std::get<abuild::ImportModuleDependency>(source.dependencies()[0]).mod).toBe(&mod);
    });

    test("ImportModulePartitionDependency", [] {
        TestProject testProject{"build_test_file",
                                {"module.cpp",
                                 "partition.cpp"}};

        abuild::Source moduleSource{testProject.projectRoot() / "module.cpp", nullptr};
        abuild::Source partitionSource{testProject.projectRoot() / "partition.cpp", nullptr};
        abuild::ModulePartition partition{"partition", &partitionSource};

        moduleSource.addDependency(abuild::ImportModulePartitionDependency{.name = "partition", .partition = &partition});

        assert_(moduleSource.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportModulePartitionDependency>(moduleSource.dependencies()[0]).name).toBe("partition");
        expect(std::get<abuild::ImportModulePartitionDependency>(moduleSource.dependencies()[0]).partition).toBe(&partition);
    });

    test("ImportSTLHeaderDependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};

        source.addDependency(abuild::ImportSTLHeaderDependency{.name = "vector"});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportSTLHeaderDependency>(source.dependencies()[0]).name).toBe("vector");
    });

    test("updating dependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "mymodule.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Source modSource{testProject.projectRoot() / "mymodule.cpp", nullptr};

        source.addDependency(abuild::ImportModuleDependency{.name = "mymodule"});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportModuleDependency>(source.dependencies()[0]).name).toBe("mymodule");
        expect(std::get<abuild::ImportModuleDependency>(source.dependencies()[0]).mod).toBe(nullptr);

        abuild::Module mod{"mymodule", &modSource};
        std::get<abuild::ImportModuleDependency>(source.dependencies()[0]).mod = &mod;

        expect(std::get<abuild::ImportModuleDependency>(source.dependencies()[0]).mod).toBe(&mod);
    });
});
