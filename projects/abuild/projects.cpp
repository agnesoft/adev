#ifdef _MSC_VER
module;

#    include <vector>

export module abuild : projects;

import : project_scanner;
#endif

namespace abuild
{
export class Projects
{
public:
    Projects(const std::filesystem::path &projectRoot, BuildCache &cache, const Settings &settings) :
        mBuildCache{cache}
    {
        ProjectScanner{projectRoot, cache, settings};
    }

    [[nodiscard]] auto sources() -> rapidjson::Value &
    {
        return mBuildCache["sources"];
    }

    [[nodiscard]] auto headers() -> rapidjson::Value &
    {
        return mBuildCache["headers"];
    }

    [[nodiscard]] auto projects() -> rapidjson::Value &
    {
        return mBuildCache["projects"];
    }

private:
    BuildCache &mBuildCache;
};
}
