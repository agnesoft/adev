#ifdef _MSC_VER
export module abuild : environment_scanner;

import : build_cache;

import acore;
#endif

namespace abuild
{
export class EnvironmentScanner
{
public:
    EnvironmentScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        mBuildCache.data().AddMember("toolchains", rapidjson::Value{rapidjson::kObjectType}, mBuildCache.data().GetAllocator());
        auto &toolchains = mBuildCache.data()["toolchains"];
        toolchains.AddMember("msvc", rapidjson::Value{rapidjson::kObjectType}, mBuildCache.data().GetAllocator());
        scan();
    }

private:
    auto
        detectWindowsClang() -> void
    {
    }

    auto detectMSVC() -> void
    {
        const std::filesystem::path msvc2019root = "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/";

        if (std::filesystem::exists(msvc2019root))
        {
            for (const std::filesystem::directory_entry &version : std::filesystem::directory_iterator(msvc2019root))
            {
                mBuildCache.data()["toolchains"]["msvc"].AddMember(rapidjson::Value{version.path().string(), mBuildCache.data().GetAllocator()}, rapidjson::Value{rapidjson::kObjectType}, mBuildCache.data().GetAllocator());
            }
        }
    }

    auto scan() -> void
    {
#ifdef _MSC_VER
        scanWindows();
#else
        scanLinux();
#endif
    }

    auto scanLinux() -> void
    {
    }

    auto scanWindows() -> void
    {
        detectMSVC();
        detectWindowsClang();
    }

    BuildCache &mBuildCache;
};
}
