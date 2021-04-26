#ifdef _MSC_VER
export module abuild : settings;

import : build_cache;
#endif

namespace abuild
{
export class Settings
{
public:
    Settings(BuildCache &buildCache) :
        mBuildCache{buildCache}
    {
        mBuildCache["settings"].AddMember("cppExtensions", cppExtensions(), mBuildCache.allocator());
    }

private:
    [[nodiscard]] static auto cppExtensions() -> rapidjson::Value
    {
        rapidjson::Value extensions{rapidjson::kArrayType};
        extensions.PushBack(".cpp", mBuildCache.allocator());
        extensions.PushBack(".cxx", mBuildCache.allocator());
        extensions.PushBack(".cc", mBuildCache.allocator());
        extensions.PushBack(".ixx", mBuildCache.allocator());
        return extensions;
    }

    BuildCache &mBuildCache;
};
}
