#ifdef _MSC_VER
export module abuild_test_utilities : test_project;

export import<astl.hpp>;
#endif

export class TestProject
{
public:
    TestProject() = default;

    TestProject(const std::filesystem::path &root, const std::vector<std::filesystem::path> &paths) :
        mProjectRoot{root}
    {
        remove_all(mProjectRoot);
        std::filesystem::create_directories(mProjectRoot);
        mProjectRoot = std::filesystem::canonical(mProjectRoot);

        for (const std::filesystem::path &path : paths)
        {
            const std::filesystem::path file = mProjectRoot / path;
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

protected:
    [[nodiscard]] auto getRoot() noexcept -> std::filesystem::path &
    {
        return mProjectRoot;
    }

private:
    std::filesystem::path mProjectRoot;
};
