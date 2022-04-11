import atest;
import abuild.cache;
import abuild.test_utilities;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Configuration", [] { // NOLINT(cert-err58-cpp)
    test("add_dynamic_library()", [] {
        ::abuild::Toolchain toolchain;
        ::abuild::Configuration config(&toolchain, "release");
        ::abuild::Project project1{.name = "my_lib"};
        ::abuild::Project project2{.name = "Kernel"};

        ::abuild::DynamicLibrary *lib1 = config->add_dynamic_library(&project1);
        ::abuild::DynamicLibrary *lib2 = config->add_dynamic_library(&project2);

        std::vector<::abuild::DynamicLibrary *> libs;
        libs.reserve(config.dynamic_libraries().size());

        for (const std::unique_ptr<::abuild::DynamicLibrary> &lib : config.dynamic_libaries())
        {
            libs.push_back(lib.get());
        }

        expect(libs).to_be(std::vector<::abuild::DynamicLibrary>{lib1, lib2});
    });
});
