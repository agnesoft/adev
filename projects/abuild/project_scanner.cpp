#ifdef _MSC_VER
export module abuild : project_scanner;

import : build_cache;
#endif

namespace abuild
{
export class ProjectScanner
{
public:
    explicit ProjectScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        scan();
    }

private:
    [[nodiscard]] static auto isCppSource(const std::filesystem::path &path) -> bool
    {
        return std::filesystem::is_regular_file(path)
            && (path.extension() == ".cpp"
                || path.extension() == ".cxx"
                || path.extension() == ".cc"
                || path.extension() == ".ixx");
    }

    [[nodiscard]] static auto isCppHeader(const std::filesystem::path &path) -> bool
    {
        return std::filesystem::is_regular_file(path)
            && (path.extension() == ".h"
                || path.extension() == ".hxx"
                || path.extension() == ".hpp");
    }

    auto scan() -> void
    {
        const std::filesystem::path projectRoot = std::filesystem::current_path();
        scanDir(projectRoot);
    }

    auto scanDir(const std::filesystem::path &dir) -> void
    {
        for (const auto &entry : std::filesystem::directory_iterator(dir))
        {
            if (isCppSource(entry.path()))
            {
                mSources.push_back(entry.path());
            }
            else if (isCppHeader(entry.path()))
            {
                mHeaders.push_back(entry.path());
            }
            else if (std::filesystem::is_directory(entry.path()))
            {
                scanDir(entry.path());
            }
        }
    }

    std::vector<std::filesystem::path> mHeaders;
    std::vector<std::filesystem::path> mSources;
    BuildCache &mBuildCache;
};
}
