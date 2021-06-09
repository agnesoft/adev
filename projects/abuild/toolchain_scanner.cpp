#ifdef _MSC_VER
export module abuild : toolchain_scanner;
export import : build_cache;
#endif

namespace abuild
{
export class ToolchainScanner
{
public:
    explicit ToolchainScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
    }

private:
    BuildCache &mBuildCache;
};
}
