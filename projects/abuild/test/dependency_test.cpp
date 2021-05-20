import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::Dependency", [] {
    test("IncludeExternalHeader", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "rapidjson/include/rapidjson.h"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Header header{testProject.projectRoot() / "rapidjson" / "include" / "rapidjson.h", nullptr};

        source.addDependency(abuild::IncludeExternalHeader{.name = "rapidjson.h", .header = &header});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::IncludeExternalHeader>(source.dependencies()[0]).name).toBe("rapidjson.h");
        expect(std::get<abuild::IncludeExternalHeader>(source.dependencies()[0]).header).toBe(&header);
    });

    test("IncludeLocalHeader", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "myheader.h"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Header header{testProject.projectRoot() / "myheader.h", nullptr};

        source.addDependency(abuild::IncludeLocalHeader{.name = "myheader.h", .header = &header});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::IncludeLocalHeader>(source.dependencies()[0]).name).toBe("myheader.h");
        expect(std::get<abuild::IncludeLocalHeader>(source.dependencies()[0]).header).toBe(&header);
    });

    test("IncludeLocalSource", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "source1.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Source otherSource{testProject.projectRoot() / "source1.cpp", nullptr};

        source.addDependency(abuild::IncludeLocalSource{.name = "source1.cpp", .source = &otherSource});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::IncludeLocalSource>(source.dependencies()[0]).name).toBe("source1.cpp");
        expect(std::get<abuild::IncludeLocalSource>(source.dependencies()[0]).source).toBe(&otherSource);
    });

    test("IncludeSTLHeader", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};

        source.addDependency(abuild::IncludeSTLHeader{.name = "vector"});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::IncludeSTLHeader>(source.dependencies()[0]).name).toBe("vector");
    });

    test("ImportExternalHeader", [] {
        test("IncludeExternalHeader", [] {
            TestProject testProject{"build_test_file",
                                    {"main.cpp",
                                     "rapidjson/include/rapidjson.h"}};

            abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
            abuild::Header header{testProject.projectRoot() / "rapidjson" / "include" / "rapidjson.h", nullptr};

            source.addDependency(abuild::ImportExternalHeader{"rapidjson.h", &header});

            assert_(source.dependencies().size()).toBe(1u);
            expect(std::get<abuild::ImportExternalHeader>(source.dependencies()[0]).name).toBe("rapidjson.h");
            expect(std::get<abuild::ImportExternalHeader>(source.dependencies()[0]).header).toBe(&header);
        });
    });

    test("ImportLocalHeader", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "myheader.h"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Header header{testProject.projectRoot() / "myheader.h", nullptr};

        source.addDependency(abuild::ImportLocalHeader{.name = "myheader.h", .header = &header});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportLocalHeader>(source.dependencies()[0]).name).toBe("myheader.h");
        expect(std::get<abuild::ImportLocalHeader>(source.dependencies()[0]).header).toBe(&header);
    });

    test("ImportModule", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "mymodule.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Source modSource{testProject.projectRoot() / "mymodule.cpp", nullptr};
        abuild::Module mod{"mymodule", &modSource};

        source.addDependency(abuild::ImportModule{.name = "mymodule", .mod = &mod});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportModule>(source.dependencies()[0]).name).toBe("mymodule");
        expect(std::get<abuild::ImportModule>(source.dependencies()[0]).mod).toBe(&mod);
    });

    test("ImportModulePartition", [] {
        TestProject testProject{"build_test_file",
                                {"module.cpp",
                                 "partition.cpp"}};

        abuild::Source moduleSource{testProject.projectRoot() / "module.cpp", nullptr};
        abuild::Source partitionSource{testProject.projectRoot() / "partition.cpp", nullptr};
        abuild::ModulePartition partition{"partition", &partitionSource};

        moduleSource.addDependency(abuild::ImportModulePartition{.name = "partition", .partition = &partition});

        assert_(moduleSource.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportModulePartition>(moduleSource.dependencies()[0]).name).toBe("partition");
        expect(std::get<abuild::ImportModulePartition>(moduleSource.dependencies()[0]).partition).toBe(&partition);
    });

    test("ImportSTLHeader", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};

        source.addDependency(abuild::ImportSTLHeader{.name = "vector"});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportSTLHeader>(source.dependencies()[0]).name).toBe("vector");
    });

    test("updating dependency", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp",
                                 "mymodule.cpp"}};

        abuild::Source source{testProject.projectRoot() / "main.cpp", nullptr};
        abuild::Source modSource{testProject.projectRoot() / "mymodule.cpp", nullptr};

        source.addDependency(abuild::ImportModule{.name = "mymodule"});

        assert_(source.dependencies().size()).toBe(1u);
        expect(std::get<abuild::ImportModule>(source.dependencies()[0]).name).toBe("mymodule");
        expect(std::get<abuild::ImportModule>(source.dependencies()[0]).mod).toBe(nullptr);

        abuild::Module mod{"mymodule", &modSource};
        std::get<abuild::ImportModule>(source.dependencies()[0]).mod = &mod;

        expect(std::get<abuild::ImportModule>(source.dependencies()[0]).mod).toBe(&mod);
    });
});
