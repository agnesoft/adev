#ifdef _MSC_VER
module;

#    include <vector>

export module abuild : project_scanner;

import : settings;
#endif

namespace abuild
{
class ProjectScanner
{
public:
    ProjectScanner(BuildCache &cache, const Settings &settings) :
        mBuildCache{cache},
        mSettings{settings}
    {
        scan();
    }

private:
    auto addHeader(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        mBuildCache["headers"].AddMember(rapidjson::Value{path.string(), mBuildCache.allocator()}, newHeader(path, projectName), mBuildCache.allocator());
        project(projectName)["headers"].PushBack(rapidjson::Value{path.string(), mBuildCache.allocator()}, mBuildCache.allocator());
    }

    auto addSource(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        mBuildCache["sources"].AddMember(rapidjson::Value{path.string(), mBuildCache.allocator()}, newSource(path, projectName), mBuildCache.allocator());
        project(projectName)["sources"].PushBack(rapidjson::Value{path.string(), mBuildCache.allocator()}, mBuildCache.allocator());
    }

    auto appendProjectName(std::string *projectName, const std::string &directoryName) -> void
    {
        if (!projectName->empty())
        {
            projectName->append(mSettings.projectNameSeparator().GetString());
        }

        projectName->append(directoryName);
    }

    [[nodiscard]] auto isCppSource(const std::filesystem::path &path) -> bool
    {
        return std::find(mSettings.cppSourceExtensions().Begin(),
                         mSettings.cppSourceExtensions().End(),
                         path.extension().string())
            != mSettings.cppSourceExtensions().End();
    }

    [[nodiscard]] auto isCppHader(const std::filesystem::path &path) -> bool
    {
        return std::find(mSettings.cppHeaderExtensions().Begin(),
                         mSettings.cppHeaderExtensions().End(),
                         path.extension().string())
            != mSettings.cppHeaderExtensions().End();
    }

    [[nodiscard]] auto isIgnoreDirectory(const std::filesystem::path &path) -> bool
    {
        return path.filename().string().front() == '.'
            || std::find(mSettings.ignoreDirectories().Begin(),
                         mSettings.ignoreDirectories().End(),
                         path.filename().string())
            != mSettings.ignoreDirectories().End();
    }

    [[nodiscard]] auto isSquashDirectory(const std::filesystem::path &path) -> bool
    {
        return std::find(mSettings.squashDirectories().Begin(),
                         mSettings.squashDirectories().End(),
                         path.filename().string())
            != mSettings.squashDirectories().End();
    }

    [[nodiscard]] auto isSkipDirectory(const std::filesystem::path &path) -> bool
    {
        return std::find(mSettings.skipDirectories().Begin(),
                         mSettings.skipDirectories().End(),
                         path.filename().string())
            != mSettings.skipDirectories().End();
    }

    [[nodiscard]] auto isStopDirectory(const std::filesystem::path &path) -> bool
    {
        return isSquashDirectory(path) || isTestDirectory(path.parent_path());
    }

    [[nodiscard]] auto isTestDirectory(const std::filesystem::path &path) -> bool
    {
        return std::find(mSettings.testDirectories().Begin(),
                         mSettings.testDirectories().End(),
                         path.filename().string())
            != mSettings.testDirectories().End();
    }

    [[nodiscard]] static auto lastModified(const std::filesystem::path &path) -> std::int64_t
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(path).time_since_epoch()).count();
    }

    [[nodiscard]] auto newHeader(const std::filesystem::path &path, const std::string &projectName) -> rapidjson::Value
    {
        rapidjson::Value project{rapidjson::kObjectType};
        project.AddMember("project", rapidjson::Value{projectName, mBuildCache.allocator()}, mBuildCache.allocator());
        project.AddMember("modified", rapidjson::Value{lastModified(path)}, mBuildCache.allocator());
        return project;
    }

    [[nodiscard]] auto newProject() -> rapidjson::Value
    {
        rapidjson::Value project{rapidjson::kObjectType};
        project.AddMember("sources", rapidjson::Value{rapidjson::kArrayType}, mBuildCache.allocator());
        project.AddMember("headers", rapidjson::Value{rapidjson::kArrayType}, mBuildCache.allocator());
        return project;
    }

    [[nodiscard]] auto newSource(const std::filesystem::path &path, const std::string &projectName) -> rapidjson::Value
    {
        rapidjson::Value project{rapidjson::kObjectType};
        project.AddMember("project", rapidjson::Value{projectName, mBuildCache.allocator()}, mBuildCache.allocator());
        project.AddMember("modified", rapidjson::Value{lastModified(path)}, mBuildCache.allocator());
        return project;
    }

    [[nodiscard]] static auto pathDirectories(std::filesystem::path path) -> std::vector<std::filesystem::path>
    {
        std::vector<std::filesystem::path> directories;

        while (path != std::filesystem::current_path())
        {
            directories.push_back(path);
            path = path.parent_path();
        }

        return directories;
    }

    auto processDir(const std::filesystem::path &path) -> void
    {
        if (isIgnoreDirectory(path))
        {
            return;
        }

        scanDir(path);
    }

    auto processFile(const std::filesystem::path &path, const std::string &project) -> void
    {
        if (isCppSource(path))
        {
            addSource(path, project);
        }
        else if (isCppHader(path))
        {
            addHeader(path, project);
        }
    }

    [[nodiscard]] auto projectNameFromDirectories(const std::vector<std::filesystem::path> &directories) -> std::string
    {
        std::string projectName;

        for (auto it = directories.crbegin(); it != directories.crend() && !isStopDirectory(*it); ++it)
        {
            if (isSkipDirectory(*it))
            {
                continue;
            }

            appendProjectName(&projectName, it->filename().string());
        }

        return projectName;
    }

    [[nodiscard]] auto projectNameFromPath(const std::filesystem::path &path) -> std::string
    {
        std::string projectName = projectNameFromDirectories(pathDirectories(path));

        if (!projectName.empty())
        {
            return projectName;
        }
        else
        {
            return std::filesystem::current_path().filename().string();
        }
    }

    auto scan() -> void
    {
        const std::filesystem::path projectRoot = std::filesystem::current_path();
        scanDir(projectRoot);
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

    auto project(const std::string &name) -> rapidjson::Value &
    {
        if (!mBuildCache["projects"].HasMember(name))
        {
            mBuildCache["projects"].AddMember(rapidjson::Value{name, mBuildCache.allocator()}, newProject(), mBuildCache.allocator());
        }

        return mBuildCache["projects"][name];
    }

    BuildCache &mBuildCache;
    const Settings &mSettings;
};
}