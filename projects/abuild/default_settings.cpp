#ifdef _MSC_VER
module abuild : default_settings;

import : build_cache;
#endif

namespace abuild
{
class DefaultSettings
{
public:
    explicit DefaultSettings(BuildCache &cache) :
        mBuildCache{cache}
    {
        if (!mBuildCache.data().HasMember("settings"))
        {
            mBuildCache["settings"].AddMember("cpp_source_extensions", cppSourceExtensions(), mBuildCache.allocator());
            mBuildCache["settings"].AddMember("cpp_header_extensions", cppHeaderExtensions(), mBuildCache.allocator());
            mBuildCache["settings"].AddMember("ignore_directories", ignoreDirectories(), mBuildCache.allocator());
            mBuildCache["settings"].AddMember("skip_directories", skipDirectories(), mBuildCache.allocator());
            mBuildCache["settings"].AddMember("squash_directories", squashDirectories(), mBuildCache.allocator());
            mBuildCache["settings"].AddMember("test_directories", testDirectories(), mBuildCache.allocator());
            mBuildCache["settings"].AddMember("project_name_separator", ".", mBuildCache.allocator());
        }
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

    [[nodiscard]] auto cppHeaderExtensions() -> rapidjson::Value
    {
        rapidjson::Value extensions{rapidjson::kArrayType};
        extensions.PushBack(".hpp", mBuildCache.allocator());
        extensions.PushBack(".hxx", mBuildCache.allocator());
        extensions.PushBack(".h", mBuildCache.allocator());
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
}
