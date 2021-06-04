#ifdef _MSC_VER
export module abuild : file;
export import : dependency;
#endif

namespace abuild
{
export class Project;

export class File
{
public:
    File(const std::filesystem::path &path, Project *project) :
        File{path, project, lastModified(path)}
    {
    }

    File(const std::filesystem::path &path, Project *project, std::int64_t timestamp) :
        mPath{std::filesystem::canonical(path)},
        mProject{project},
        mTimestamp{timestamp}
    {
    }

    auto addDependency(Dependency dependency) -> void
    {
        mDependencies.push_back(std::move(dependency));
    }

    [[nodiscard]] auto content() const noexcept -> std::string
    {
        std::ifstream file{mPath};
        std::string data;

        file.seekg(0, std::ios::end);
        data.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        data.assign(std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{});

        return data;
    }

    [[nodiscard]] auto dependencies() noexcept -> std::vector<Dependency> &
    {
        return mDependencies;
    }

    [[nodiscard]] auto extension() const noexcept -> std::string
    {
        return path().filename().extension().string();
    }

    [[nodiscard]] auto isModified() const -> bool
    {
        return timestamp() != lastModified(mPath);
    }

    [[nodiscard]] auto name() const -> std::string
    {
        return path().filename().string();
    }

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &
    {
        return mPath;
    }

    [[nodiscard]] auto project() noexcept -> Project *
    {
        return mProject;
    }

    [[nodiscard]] auto project() const noexcept -> const Project *
    {
        return mProject;
    }

    [[nodiscard]] auto timestamp() const noexcept -> std::int64_t
    {
        return mTimestamp;
    }

    auto update() -> void
    {
        mTimestamp = lastModified(mPath);
    }

private:
    [[nodiscard]] static auto lastModified(const std::filesystem::path &path) -> std::int64_t
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(path).time_since_epoch()).count();
    }

    std::filesystem::path mPath;
    Project *mProject = nullptr;
    std::int64_t mTimestamp = 0;
    std::vector<Dependency> mDependencies;
};
}
