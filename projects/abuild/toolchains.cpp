#ifdef _MSC_VER
export module abuild : toolchains;
import : build_cache;
#endif

namespace abuild
{
export class Toolchains
{
public:
    explicit Toolchains(BuildCache &cache) :
        mBuildCache{cache}
    {
    }

    [[nodiscard]] auto toolchains() -> rapidjson::Value &
    {
        return mBuildCache["toolchains"];
    }

private:
    BuildCache &mBuildCache;
};
}
