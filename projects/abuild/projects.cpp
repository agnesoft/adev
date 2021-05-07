#ifdef _MSC_VER
module;
#    include <vector>
export module abuild : projects;
import : build_cache;
#endif

namespace abuild
{
export class Projects
{
public:
    Projects(BuildCache &cache) :
        mBuildCache{cache}
    {
    }

    [[nodiscard]] auto modules() -> rapidjson::Value &
    {
        return mBuildCache["modules"];
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
