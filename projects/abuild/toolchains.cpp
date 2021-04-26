#ifdef _MSC_VER
export module abuild : toolchains;

import : toolchains_scanner_windows;
#else
#    include "toolchains_scanner_linux.cpp"
#endif

namespace abuild
{
export class Toolchains
{
public:
    explicit Toolchains(BuildCache &cache) :
        mBuildCache{cache}
    {
        ToolchainsScanner{cache};
    }

private:
    BuildCache &mBuildCache;
};
}
