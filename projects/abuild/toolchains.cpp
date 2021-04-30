#ifdef _MSC_VER
export module abuild : toolchains;

import : toolchain_scanner_windows;
#else
#    include "toolchain_scanner_linux.cpp"
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
