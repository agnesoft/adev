import abuild;
import abuild_test_utilities;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

static const auto testSuite = suite("abuild::File", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::BuildCache>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::BuildCache>).toBe(false);
        expect(std::is_nothrow_move_constructible_v<abuild::BuildCache>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::BuildCache>).toBe(false);
        expect(std::is_nothrow_move_assignable_v<abuild::BuildCache>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::BuildCache>).toBe(true);
    });

    test("missing file", [] {
        expect([] { abuild::File{"missing_file"}; }).toThrow<std::filesystem::filesystem_error>();
    });

    test("extension", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        const abuild::File file{testProject.projectRoot() / "main.cpp"};

        expect(file.extension()).toBe(".cpp");
    });

    test("name", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        const abuild::File file{testProject.projectRoot() / "main.cpp"};

        expect(file.name()).toBe("main.cpp");
    });

    test("path", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        const std::filesystem::path path = testProject.projectRoot() / "main.cpp";
        const abuild::File file{path};

        expect(file.path()).toBe(path);
    });

    test("project", [] {
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        abuild::File file{testProject.projectRoot() / "main.cpp"};

        expect(std::as_const(file).project()).toBe(nullptr);

        abuild::Project project;
        file.setProject(&project);

        expect(std::as_const(file).project()).toBe(&project);
    });

    test("timestamp", [] {
        using namespace std::chrono_literals;

        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        const std::filesystem::path path = testProject.projectRoot() / "main.cpp";
        auto getTimestamp = [&]() { return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(path).time_since_epoch()).count(); };
        const std::int64_t lastModified = getTimestamp();

        const abuild::File file{testProject.projectRoot() / "main.cpp"};

        expect(file.timestamp()).toBe(lastModified);

        std::filesystem::last_write_time(path, std::filesystem::last_write_time(path) + 1h);

        expect(file.timestamp()).toBe(lastModified);
    });

    test("isModified", [] {
        using namespace std::chrono_literals;

        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        const std::filesystem::path path = testProject.projectRoot() / "main.cpp";

        const abuild::File file{testProject.projectRoot() / "main.cpp"};

        expect(file.isModified()).toBe(false);

        std::filesystem::last_write_time(path, std::filesystem::last_write_time(path) + 1h);

        expect(file.isModified()).toBe(true);
    });

    test("update", [] {
        using namespace std::chrono_literals;

        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        const std::filesystem::path path = testProject.projectRoot() / "main.cpp";

        abuild::File file{testProject.projectRoot() / "main.cpp"};

        expect(file.isModified()).toBe(false);

        std::filesystem::last_write_time(path, std::filesystem::last_write_time(path) + 1h);
        file.update();

        expect(file.isModified()).toBe(false);
    });

    test("content", [] {
        TestProjectWithContent testProject{"build_test_project_scanner",
                                           {{"main.cpp", "auto main(int argc, char *argv[])\n{\n    return 0;\n}\n"}}};

        const abuild::File file{testProject.projectRoot() / "main.cpp"};

        expect(file.content()).toBe("auto main(int argc, char *argv[])\n{\n    return 0;\n}\n");
    });
});
