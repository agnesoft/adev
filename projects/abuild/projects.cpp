#ifdef _MSC_VER
export module abuild : projects;

import : projects_scanner;
#endif

namespace abuild
{
export class Projects
{
public:
    explicit Projects(BuildCache &cache, const Settings &settings)
    {
        ProjectsScanner{cache, settings};
    }
};
}
