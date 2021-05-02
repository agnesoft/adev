import abuild;
import abuild_test_cache;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

namespace
{
class TestProject
{
public:
    explicit TestProject(const std::filesystem::path &projectRoot, const std::vector<std::filesystem::path> &paths) :
        mProjectRoot{projectRoot}
    {
        std::filesystem::create_directories(mProjectRoot);

        for (const std::filesystem::path &path : paths)
        {
            const std::filesystem::path file = projectRoot / path;
            std::filesystem::create_directories(file.parent_path());
            std::ofstream{file};
        }
    }

    TestProject(const TestProject &other) = delete;
    TestProject(TestProject &&other) noexcept = default;

    ~TestProject()
    {
        try
        {
            remove_all(mProjectRoot);
        }
        catch (...)
        {
            try
            {
                std::cout << "Failed to remove '" << mProjectRoot << "' directory.\n";
            }
            catch (...)
            {
            }
        }
    }

    [[nodiscard]] auto projectRoot() const noexcept -> const std::filesystem::path &
    {
        return mProjectRoot;
    }

    auto operator=(const TestProject &other) -> TestProject & = delete;
    auto operator=(TestProject &&other) noexcept -> TestProject & = default;

private:
    const std::filesystem::path mProjectRoot;
};
}

static const auto testSuite = suite("abuild::Projects", [] {
    test("type traits", [] {
        expect(std::is_default_constructible_v<abuild::Projects>).toBe(false);
        expect(std::is_copy_constructible_v<abuild::Projects>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<abuild::Projects>).toBe(true);
        expect(std::is_copy_assignable_v<abuild::Projects>).toBe(false);
        expect(std::is_nothrow_move_assignable_v<abuild::Projects>).toBe(false);
        expect(std::is_nothrow_destructible_v<abuild::Projects>).toBe(true);
    });

    test("single source", [] {
        TestCache testCache;
        TestProject testProject{"build_test_project_scanner",
                                {"main.cpp"}};

        {
            abuild::BuildCache cache{testCache.file()};
            abuild::Settings settings{cache};
            abuild::Projects projects{testProject.projectRoot(), cache, settings};

            expect(projects.projects().HasMember("build_test_project_scanner")).toBe(true);

            const std::string sourceFile = std::filesystem::canonical(std::filesystem::path{"build_test_project_scanner"} / "main.cpp").string();
            expect(projects.sources().HasMember(sourceFile)).toBe(true);
        }
    });
});
