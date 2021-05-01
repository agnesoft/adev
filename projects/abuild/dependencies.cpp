#ifdef _MSC_VER
export module abuild : dependencies;

import : code_scanner;
#endif

namespace abuild
{
export class Dependencies
{
public:
    explicit Dependencies(BuildCache &cache)
    {
        CodeScanner{cache};
    }
};
}
