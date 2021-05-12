#ifdef _MSC_VER
export module abuild : file;
import<astl.hpp>;
#endif

namespace abuild
{
export class Project;

export class File
{
public:
    explicit File(const std::filesystem::path &path) :
        mPath{std::filesystem::canonical(path)},
        mTimestamp{lastModified()}
    {
    }

    explicit File(const std::filesystem::path &path, std::int64_t timestamp) :
        mPath{std::filesystem::canonical(path)},
        mTimestamp{timestamp}
    {
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

    [[nodiscard]] auto extension() const noexcept -> std::string
    {
        return path().filename().extension().string();
    }

    [[nodiscard]] auto isModified() const -> bool
    {
        return timestamp() != lastModified();
    }

    [[nodiscard]] auto name() const -> std::string
    {
        return path().filename().string();
    }

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &
    {
        return mPath;
    }

    [[nodiscard]] auto project() const noexcept -> const Project *
    {
        return mProject;
    }

    auto setProject(Project *project) noexcept -> void
    {
        mProject = project;
    }

    [[nodiscard]] auto timestamp() const noexcept -> std::int64_t
    {
        return mTimestamp;
    }

    auto update() -> void
    {
        mTimestamp = lastModified();
    }

private:
    [[nodiscard]] auto lastModified() const -> std::int64_t
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(mPath).time_since_epoch()).count();
    }

    std::filesystem::path mPath;
    std::int64_t mTimestamp;
    Project *mProject = nullptr;
};
}
