#ifdef _MSC_VER
export module abuild : sdk_scanner;
export import : build_cache;
#endif

namespace abuild
{
export class SDKScanner
{
public:
    explicit SDKScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        detectWindowsSDK();
    }

private:
    auto detectWindowsSDK() -> void
    {
    }

    BuildCache &mBuildCache;
};
}
