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
    auto addSource(const std::filesystem::path &path) -> void
    {
        mBuildCache["sources"].AddMember(rapidjson::Value{path.string(), mBuildCache.allocator()}, rapidjson::Value{rapidjson::kObjectType}, mBuildCache.allocator());
    }

    auto addSourceToProject(const std::filesystem::path &path) -> void
    {
        currentProject(path)["sources"].PushBack(rapidjson::Value{path.string(), mBuildCache.allocator()}, mBuildCache.allocator());
    }

    [[nodiscard]] auto currentProject(const std::filesystem::path &path) -> rapidjson::Value &
    {
        if (!mCurrentProject)
        {
            setCurrentProject(projectName(path));
        }

        return *mCurrentProject;
    }

    [[nodiscard]] auto isCppSource(const std::filesystem::path &path) -> bool
    {
        return std::find(mSettings.cppSourceExtensions().Begin(),
                         mSettings.cppSourceExtensions().End(),
                         path.extension().string())
            != mSettings.cppSourceExtensions().End();
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

    [[nodiscard]] auto isTestDirectory(const std::filesystem::path &path) -> bool
    {
        return std::find(mSettings.testDirectories().Begin(),
                         mSettings.testDirectories().End(),
                         path.filename().string())
            != mSettings.testDirectories().End();
    }

    [[nodiscard]] auto newProject() -> rapidjson::Value
    {
        rapidjson::Value project{rapidjson::kObjectType};
        project.AddMember("sources", rapidjson::Value{rapidjson::kArrayType}, mBuildCache.allocator());
        return project;
    }

    auto processDir(const std::filesystem::path &path) -> void
    {
        if (isIgnoreDirectory(path))
        {
            return;
        }

        if (!isSquashDirectory(path))
        {
            mCurrentProject = nullptr;
        }

        scanDir(path);
    }

    auto processFile(const std::filesystem::path &path) -> void
    {
        if (isCppSource(path))
        {
            addSource(path);
            addSourceToProject(path);
        }
    }

    [[nodiscard]] auto projectName(const std::filesystem::path &path) -> std::string
    {
        if (std::filesystem::is_regular_file(path) || isSquashDirectory(path) || isSkipDirectory(path))
        {
            return projectName(path.parent_path());
        }

        if (isTestDirectory(path))
        {
            return projectName(path.parent_path()) + projectNameSeparator(path) + path.filename().string();
        }

        return path.filename().string();
    }

    [[nodiscard]] static auto projectNameSeparator(const std::filesystem::path &path) -> const char *
    {
        if (std::islower(path.filename().string().front()))
        {
            return "_";
        }

        return "";
    }

    auto scan() -> void
    {
        const std::filesystem::path projectRoot = std::filesystem::current_path();
        scanDir(projectRoot);
    }

    auto scanDir(const std::filesystem::path &projectRoot) -> void
    {
        for (const auto &entry : std::filesystem::directory_iterator(projectRoot))
        {
            const std::filesystem::path path = entry.path();

            if (std::filesystem::is_regular_file(path))
            {
                processFile(path);
            }
            else if (std::filesystem::is_directory(path))
            {
                processDir(path);
            }
        }
    }

    auto setCurrentProject(const std::string &name) -> void
    {
        if (!mBuildCache["projects"].HasMember(name))
        {
            mBuildCache["projects"].AddMember(rapidjson::Value{name, mBuildCache.allocator()}, newProject(), mBuildCache.allocator());
        }

        mCurrentProject = &mBuildCache["projects"][name];
    }

    BuildCache &mBuildCache;
    const Settings &mSettings;
    rapidjson::Value *mCurrentProject = nullptr;
};
}
