#ifdef _MSC_VER
export module abuild : settings;

import : default_settings;
#endif

namespace abuild
{
export class Settings
{
public:
    explicit Settings(BuildCache &cache) :
        mBuildCache{cache}
    {
        DefaultSettings{cache};
    }

    [[nodiscard]] auto cppSourceExtensions() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["cpp_source_extensions"];
    }

    [[nodiscard]] auto cppHeaderExtensions() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["cpp_header_extensions"];
    }

    [[nodiscard]] auto ignoreDirectories() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["ignore_directories"];
    }

    [[nodiscard]] auto projectNameSeparator() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["project_name_separator"];
    }

    [[nodiscard]] auto skipDirectories() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["skip_directories"];
    }

    [[nodiscard]] auto squashDirectories() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["squash_directories"];
    }

    [[nodiscard]] auto testDirectories() const -> const rapidjson::Value &
    {
        return mBuildCache["settings"]["test_directories"];
    }

private:
    BuildCache &mBuildCache;
};
}
