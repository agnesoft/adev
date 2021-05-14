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
        expect(std::is_default_constructible_v<abuild::File>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::File>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::File>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::File>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<abuild::File>).toBe(true);
        expect(std::is_nothrow_destructible_v<abuild::File>).toBe(true);
    });

    test("missing file", [] {
        expect([] { abuild::File{"missing_file", nullptr}; }).toThrow<std::filesystem::filesystem_error>();
    });

    test("extension", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        abuild::Project project{"myproject"};
        const abuild::File file{testProject.projectRoot() / "main.cpp", &project};

        expect(file.extension()).toBe(".cpp");
    });

    test("name", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        abuild::Project project{"myproject"};
        const abuild::File file{testProject.projectRoot() / "main.cpp", &project};

        expect(file.name()).toBe("main.cpp");
    });

    test("project", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        abuild::Project project{"myproject"};
        const abuild::File file{testProject.projectRoot() / "main.cpp", &project};

        expect(file.project()).toBe(&project);
    });

    test("path", [] {
        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        const std::filesystem::path path = testProject.projectRoot() / "main.cpp";
        abuild::Project project{"myproject"};
        const abuild::File file{path, &project};

        expect(file.path()).toBe(path);
    });

    test("timestamp", [] {
        using namespace std::chrono_literals;

        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        const std::filesystem::path path = testProject.projectRoot() / "main.cpp";
        auto getTimestamp = [&]() { return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(path).time_since_epoch()).count(); };
        const std::int64_t lastModified = getTimestamp();

        abuild::Project project{"myproject"};
        const abuild::File file{testProject.projectRoot() / "main.cpp", &project};

        expect(file.timestamp()).toBe(lastModified);

        std::filesystem::last_write_time(path, std::filesystem::last_write_time(path) + 1h);

        expect(file.timestamp()).toBe(lastModified);
    });

    test("custom timestamp", [] {
        using namespace std::chrono_literals;

        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        const std::filesystem::path path = testProject.projectRoot() / "main.cpp";
        const std::int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>((std::filesystem::last_write_time(path) - 1h).time_since_epoch()).count();
        abuild::Project project{"myproject"};
        const abuild::File file{testProject.projectRoot() / "main.cpp", &project, timestamp};

        expect(file.timestamp()).toBe(timestamp);
    });

    test("isModified", [] {
        using namespace std::chrono_literals;

        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        const std::filesystem::path path = testProject.projectRoot() / "main.cpp";

        abuild::Project project{"myproject"};
        const abuild::File file{testProject.projectRoot() / "main.cpp", &project};

        expect(file.isModified()).toBe(false);

        std::filesystem::last_write_time(path, std::filesystem::last_write_time(path) + 1h);

        expect(file.isModified()).toBe(true);
    });

    test("update", [] {
        using namespace std::chrono_literals;

        TestProject testProject{"build_test_file",
                                {"main.cpp"}};

        const std::filesystem::path path = testProject.projectRoot() / "main.cpp";

        abuild::Project project{"myproject"};
        abuild::File file{testProject.projectRoot() / "main.cpp", &project};

        expect(file.isModified()).toBe(false);

        std::filesystem::last_write_time(path, std::filesystem::last_write_time(path) + 1h);
        file.update();

        expect(file.isModified()).toBe(false);
    });

    test("content", [] {
        TestProjectWithContent testProject{"build_test_file",
                                           {{"main.cpp", "auto main(int argc, char *argv[])\n{\n    return 0;\n}\n"}}};

        abuild::Project project{"myproject"};
        const abuild::File file{testProject.projectRoot() / "main.cpp", &project};

        expect(file.content()).toBe("auto main(int argc, char *argv[])\n{\n    return 0;\n}\n");
    });
});
