#ifdef _MSC_VER
export module abuild : toolchains_scanner_windows;

import : dependency_scanner;
#endif

namespace abuild
{
class Dependencies
{
public:
    explicit Dependencies(BuildCache &cache)
    {
        DependencyScanner{cache};
    }
};
}
