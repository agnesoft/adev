#ifdef _MSC_VER
export module abuild : dependency_scanner;

import : build_cache;
#endif

namespace abuild
{
class DependencyScanner
{
public:
    explicit DependencyScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
    }

private:
    BuildCache &mBuildCache;
};
}
