#ifndef __clang__
export module abuild.test_utilities : test_project;
export import : test_file;
#endif

namespace abuild
{
export class ProjectRoot
{
public:
    explicit ProjectRoot(std::filesystem::path path) :
        root{std::move(path)}
    {
        if (!std::filesystem::exists(this->root))
        {
            std::filesystem::create_directories(this->root);
        }
    }

    ProjectRoot(const ProjectRoot &other) = delete;
    ProjectRoot(ProjectRoot &&other) noexcept = default;

    ~ProjectRoot()
    {
        std::error_code ec{};
        std::filesystem::remove_all(this->root, ec);
    }

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &
    {
        return this->root;
    }

    auto operator=(const ProjectRoot &other) -> ProjectRoot & = delete;
    auto operator=(ProjectRoot &&other) noexcept -> ProjectRoot & = default;

private:
    std::filesystem::path root;
};

//! \private
export class TestProject
{
public:
    TestProject(std::string_view root, const std::vector<std::pair<std::string_view, std::string_view>> &files) :
        projectRoot{root}
    {
        this->projectFiles.reserve(files.size());

        for (const auto &file : files)
        {
            std::filesystem::path path{this->projectRoot.path() / file.first};
            std::filesystem::create_directories(path.parent_path());
            this->projectFiles.emplace_back(TestFile{std::move(path), file.second});
        }
    }

    [[nodiscard]] auto root() const noexcept -> const std::filesystem::path &
    {
        return this->projectRoot.path();
    }

private:
    ProjectRoot projectRoot;
    std::vector<TestFile> projectFiles;
};
}
