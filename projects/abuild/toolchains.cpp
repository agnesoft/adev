#ifdef _MSC_VER
export module abuild : toolchains;

import : toolchain_scanner;
#endif

namespace abuild
{
export class Toolchains
{
public:
    explicit Toolchains(BuildCache &cache) :
        mBuildCache{cache}
    {
        ToolchainScanner{cache};
    }

    [[nodiscard]] auto toolchains() -> rapidjson::Value &
    {
        return mBuildCache["toolchains"];
    }

private:
    BuildCache &mBuildCache;
};
}
