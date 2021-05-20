#ifdef _MSC_VER
export module abuild : project_scanner;
export import : build_cache;
import : settings;
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
        scanDirectory(mProjectRoot);
    }

private:
    static auto appendProjectName(std::string *projectName, const std::string &directoryName) -> void
    {
        if (projectName->empty())
        {
            *projectName = directoryName;
        }
        else
        {
            *projectName += Settings::projectNameSeparator() + directoryName;
        }
    }

    [[nodiscard]] auto ensureProjectName(const std::string &projectName) const -> std::string
    {
        if (projectName.empty())
        {
            return mProjectRoot.filename().string();
        }
        else
        {
            return projectName;
        }
    }

    [[nodiscard]] auto isIgnoreDirectory(const std::filesystem::path &path) -> bool
    {
        return path.filename().string().front() == '.' || Settings::ignoreDirectories().contains(path.filename().string());
    }

    [[nodiscard]] auto isSkipDirectory(const std::filesystem::path &path) -> bool
    {
        return Settings::skipDirectories().contains(path.filename().string());
    }

    [[nodiscard]] auto isSquashDirectory(const std::filesystem::path &path) -> bool
    {
        return Settings::squashDirectories().contains(path.filename().string());
    }

    [[nodiscard]] auto isTestDirectory(const std::filesystem::path &path) -> bool
    {
        return Settings::testDirectories().contains(path.filename().string());
    }

    [[nodiscard]] auto pathDirectories(std::filesystem::path path) -> std::vector<std::filesystem::path>
    {
        std::vector<std::filesystem::path> directories;

        while (path != mProjectRoot)
        {
            directories.push_back(path);
            path = path.parent_path();
        }

        return directories;
    }

    auto processFile(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        if (Settings::cppSourceExtensions().contains(path.extension().string()))
        {
            mBuildCache.addSource(path, projectName);
        }
        else if (Settings::cppHeaderExtensions().contains(path.extension().string()))
        {
            mBuildCache.addHeader(path, projectName);
        }
    }

    [[nodiscard]] auto projectNameFromDirectories(const std::vector<std::filesystem::path> &directories) -> std::string
    {
        std::string projectName;

        for (auto it = directories.crbegin(); it != directories.crend() && !isSquashDirectory(*it); ++it)
        {
            if (isTestDirectory(*it))
            {
                return ensureProjectName(projectName) + Settings::projectNameSeparator() + it->filename().string();
            }
            else if (!isSkipDirectory(*it))
            {
                appendProjectName(&projectName, it->filename().string());
            }
        }

        return ensureProjectName(projectName);
    }

    [[nodiscard]] auto projectNameFromPath(const std::filesystem::path &path) -> std::string
    {
        return projectNameFromDirectories(pathDirectories(path));
    }

    auto scanDirectoryEntry(const std::filesystem::directory_entry &entry, const std::string &projectName) -> void
    {
        const std::filesystem::path entryPath = entry.path();

        if (std::filesystem::is_regular_file(entryPath))
        {
            processFile(entryPath, projectName);
        }
        else if (std::filesystem::is_directory(entryPath) && !isIgnoreDirectory(entryPath))
        {
            scanDirectory(entryPath);
        }
    }

    auto scanDirectory(const std::filesystem::path &path) -> void
    {
        const std::string projectName = projectNameFromPath(path);

        for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(path))
        {
            scanDirectoryEntry(entry, projectName);
        }
    }

    BuildCache &mBuildCache;
    std::filesystem::path mProjectRoot;
};
}
