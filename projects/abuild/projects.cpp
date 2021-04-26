#ifdef _MSC_VER
export module abuild : projects;

import : settings;
#endif

namespace abuild
{
export class Projects
{
public:
    explicit Projects(BuildCache &cache, const Settings &settings) :
        mBuildCache{cache},
        mSettings{settings}
    {
        scan();
    }

private:
    [[nodiscard]] static auto isCppSource(const std::filesystem::path &path) -> bool
    {
        return std::filesystem::is_regular_file(path);
    }

    [[nodiscard]] static auto isSouce(const std::filesystem::path &path) -> bool
    {
        return isCppSource(path) || isCppHeader(path);
    }

    auto scan() -> void
    {
        const std::filesystem::path projectRoot = std::filesystem::current_path();
        detectProject(projectRoot);
    }

    auto detectProject(const std::filesystem::path &projectRoot) -> void
    {
        for (const auto &entry : std::filesystem::directory_iterator(projectRoot))
        {
        }
    }

    BuildCache &mBuildCache;
    const Settings &mSettings;
};
}
