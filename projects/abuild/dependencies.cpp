#ifdef _MSC_VER
export module abuild : dependencies;

import : dependency_scanner;
#endif

namespace abuild
{
export class Dependencies
{
public:
    explicit Dependencies(BuildCache &cache)
    {
        DependencyScanner{cache};
    }
};
}
