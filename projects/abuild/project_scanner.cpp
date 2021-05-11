#ifdef _MSC_VER
module;
#    include <vector>
export module abuild : project_scanner;
import : build_cache;
#endif

namespace abuild
{
export class ProjectScanner
{
public:
    ProjectScanner(BuildCache &cache, const std::filesystem::path &projectRoot) :
        mProjectRoot{std::filesystem::canonical(projectRoot)},
        mBuildCache{cache}
    {
        scanDir(std::filesystem::canonical(projectRoot));
    }

private:
    auto addHeader(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        mBuildCache.headers().AddMember(rapidjson::Value{path.string(), mBuildCache.allocator()}, newHeader(path, projectName), mBuildCache.allocator());
        project(projectName)["headers"].PushBack(rapidjson::Value{path.string(), mBuildCache.allocator()}, mBuildCache.allocator());
    }

    auto addSource(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        mBuildCache.sources().AddMember(rapidjson::Value{path.string(), mBuildCache.allocator()}, newSource(path, projectName), mBuildCache.allocator());
        project(projectName)["sources"].PushBack(rapidjson::Value{path.string(), mBuildCache.allocator()}, mBuildCache.allocator());
    }

    [[nodiscard]] auto appendProjectName(std::string projectName, const std::string &directoryName) const -> std::string
    {
        if (!projectName.empty())
        {
            return projectName + mBuildCache.projectNameSeparator().GetString() + directoryName;
        }

        return projectName + directoryName;
    }

    [[nodiscard]] auto isCppSource(const std::filesystem::path &path) -> bool
    {
        return std::find(mBuildCache.cppSourceExtensions().Begin(),
                         mBuildCache.cppSourceExtensions().End(),
                         path.extension().string())
            != mBuildCache.cppSourceExtensions().End();
    }

    [[nodiscard]] auto isCppHader(const std::filesystem::path &path) -> bool
    {
        return std::find(mBuildCache.cppHeaderExtensions().Begin(),
                         mBuildCache.cppHeaderExtensions().End(),
                         path.extension().string())
            != mBuildCache.cppHeaderExtensions().End();
    }

    [[nodiscard]] static auto isMain(const std::filesystem::path &path) -> bool
    {
        return path.stem() == "main";
    }

    [[nodiscard]] auto isIgnoreDirectory(const std::filesystem::path &path) -> bool
    {
        return path.filename().string().front() == '.'
            || std::find(mBuildCache.ignoreDirectories().Begin(),
                         mBuildCache.ignoreDirectories().End(),
                         path.filename().string())
            != mBuildCache.ignoreDirectories().End();
    }

    [[nodiscard]] auto isSquashDirectory(const std::filesystem::path &path) -> bool
    {
        return std::find(mBuildCache.squashDirectories().Begin(),
                         mBuildCache.squashDirectories().End(),
                         path.filename().string())
            != mBuildCache.squashDirectories().End();
    }

    [[nodiscard]] auto isSkipDirectory(const std::filesystem::path &path) -> bool
    {
        return std::find(mBuildCache.skipDirectories().Begin(),
                         mBuildCache.skipDirectories().End(),
                         path.filename().string())
            != mBuildCache.skipDirectories().End();
    }

    [[nodiscard]] auto isStopDirectory(const std::filesystem::path &path) -> bool
    {
        return isSquashDirectory(path) || isTestDirectory(path.parent_path());
    }

    [[nodiscard]] auto isTest(const std::string &name) -> bool
    {
        size_t pos = name.find_last_of(mBuildCache.projectNameSeparator().GetString());
        return pos != std::string::npos && isTestDirectory(name.substr(pos + 1));
    }

    [[nodiscard]] auto isTestDirectory(const std::filesystem::path &path) -> bool
    {
        return std::find(mBuildCache.testDirectories().Begin(),
                         mBuildCache.testDirectories().End(),
                         path.filename().string())
            != mBuildCache.testDirectories().End();
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
        project.AddMember("type", "library", mBuildCache.allocator());
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

    auto processDir(const std::filesystem::path &path) -> void
    {
        if (isIgnoreDirectory(path))
        {
            return;
        }

        scanDir(path);
    }

    auto processFile(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        if (isCppSource(path))
        {
            addSource(path, projectName);

            if (isMain(path))
            {
                project(projectName)["type"] = "executable";
            }
        }
        else if (isCppHader(path))
        {
            addHeader(path, projectName);
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

            projectName = appendProjectName(projectName, it->filename().string());
        }

        if (isTestDirectory(projectName))
        {
            projectName = appendProjectName(mProjectRoot.filename().string(), projectName);
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
            return mProjectRoot.filename().string();
        }
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
        if (!mBuildCache.projects().HasMember(name))
        {
            mBuildCache.projects().AddMember(rapidjson::Value{name, mBuildCache.allocator()}, newProject(), mBuildCache.allocator());

            if (isTest(name))
            {
                mBuildCache.projects()[name]["type"] = "executable";
            }
        }

        return mBuildCache.projects()[name];
    }

    std::filesystem::path mProjectRoot;
    BuildCache &mBuildCache;
};
}