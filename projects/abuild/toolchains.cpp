#ifdef _MSC_VER
export module abuild : toolchains;

import : toolchain_scanner_windows;
#endif

namespace abuild
{
export class Toolchains
{
public:
    explicit Toolchains(BuildCache &cache)
    {
        ToolchainScanner{cache};
    }
};
}
