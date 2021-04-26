#ifdef _MSC_VER
export module abuild : settings;

import : build_cache;
#endif

namespace abuild
{
class DefaultSettings
{
public:
    explicit DefaultSettings(BuildCache &buildCache) :
        mBuildCache{buildCache}
    {
        mBuildCache["settings"].AddMember("cppSourceExtensions", cppSourceExtensions(), mBuildCache.allocator());
        mBuildCache["settings"].AddMember("ignoreDirectories", ignoreDirectories(), mBuildCache.allocator());
        mBuildCache["settings"].AddMember("skipDirectories", skipDirectories(), mBuildCache.allocator());
        mBuildCache["settings"].AddMember("squashDirectories", squashDirectories(), mBuildCache.allocator());
        mBuildCache["settings"].AddMember("testDirectories", testDirectories(), mBuildCache.allocator());
    }

private:
    [[nodiscard]] auto cppSourceExtensions() -> rapidjson::Value
    {
        rapidjson::Value extensions{rapidjson::kArrayType};
        extensions.PushBack(".cpp", mBuildCache.allocator());
        extensions.PushBack(".cxx", mBuildCache.allocator());
        extensions.PushBack(".cc", mBuildCache.allocator());
        extensions.PushBack(".ixx", mBuildCache.allocator());
        return extensions;
    }

    [[nodiscard]] auto ignoreDirectories() -> rapidjson::Value
    {
        rapidjson::Value directories{rapidjson::kArrayType};
        directories.PushBack("build", mBuildCache.allocator());
        return directories;
    }

    [[nodiscard]] auto skipDirectories() -> rapidjson::Value
    {
        rapidjson::Value directories{rapidjson::kArrayType};
        directories.PushBack("projects", mBuildCache.allocator());
        directories.PushBack("Projects", mBuildCache.allocator());
        return directories;
    }

    [[nodiscard]] auto squashDirectories() -> rapidjson::Value
    {
        rapidjson::Value directories{rapidjson::kArrayType};
        directories.PushBack("src", mBuildCache.allocator());
        directories.PushBack("srcs", mBuildCache.allocator());
        directories.PushBack("SRC", mBuildCache.allocator());
        directories.PushBack("Src", mBuildCache.allocator());

        directories.PushBack("source", mBuildCache.allocator());
        directories.PushBack("sources", mBuildCache.allocator());
        directories.PushBack("Source", mBuildCache.allocator());
        directories.PushBack("Sources", mBuildCache.allocator());

        directories.PushBack("include", mBuildCache.allocator());
        directories.PushBack("Include", mBuildCache.allocator());
        directories.PushBack("includes", mBuildCache.allocator());
        directories.PushBack("Includes", mBuildCache.allocator());
        return directories;
    }

    [[nodiscard]] auto testDirectories() -> rapidjson::Value
    {
        rapidjson::Value directories{rapidjson::kArrayType};
        directories.PushBack("test", mBuildCache.allocator());
        directories.PushBack("Test", mBuildCache.allocator());
        directories.PushBack("tests", mBuildCache.allocator());
        directories.PushBack("Tests", mBuildCache.allocator());
        return directories;
    }

    BuildCache &mBuildCache;
};

export class Settings : DefaultSettings
{
public:
    explicit Settings(BuildCache &buildCache) :
        DefaultSettings{buildCache},
        mBuildCache{buildCache}
    {
    }

    [[nodiscard]] auto cppSourceExtensions() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["cppSourceExtensions"];
    }

    [[nodiscard]] auto ignoreDirectories() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["ignoreDirectories"];
    }

    [[nodiscard]] auto skipDirectories() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["skipDirectories"];
    }

    [[nodiscard]] auto squashDirectories() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["squashDirectories"];
    }

    [[nodiscard]] auto testDirectories() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["testDirectories"];
    }

private:
    BuildCache &mBuildCache;
};
}
