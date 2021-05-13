#ifdef _MSC_VER
export module abuild : project_scanner;
import : build_cache;
#endif

namespace abuild
{
export class ProjectScanner
{
public:
    ProjectScanner(BuildCache &cache, const std::filesystem::path &projectRoot) :
        mBuildCache{cache},
        mProjectRoot{std::filesystem::canonical(projectRoot)}

    {
        scanDir(std::filesystem::canonical(projectRoot));
    }

    auto scanDir(const std::filesystem::path &path) -> void
    {
        const std::string projectName = projectNameFromPath(path);

        for (const auto &entry : std::filesystem::directory_iterator(path))
        {
            const std::filesystem::path entryPath = entry.path();

            if (std::filesystem::is_regular_file(entryPath))
            {
                processFile(entryPath, projectName);
            }
            else if (std::filesystem::is_directory(entryPath))
            {
                processDir(entryPath);
            }
        }
    }

private:
    BuildCache &mBuildCache;
    std::filesystem::path mProjectRoot;
};
}
