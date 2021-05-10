#ifdef _MSC_VER
export module abuild : dependency_scanner;
import : build_cache;
#endif

namespace abuild
{
export class DependencyScanner
{
public:
    explicit DependencyScanner(BuildCache &cache) :
        mBuildCache{cache}
    {
        for (rapidjson::Value::MemberIterator source = mBuildCache.sources().MemberBegin(); source != mBuildCache.sources().MemberEnd(); ++it)
        {
            scanSourceDependencies(source);
        }
    }

private:
    auto scanIncludesExternal(rapidjson::Value::MemberIterator source) -> void
    {
    }

    auto scanIncludesLocal(rapidjson::Value::MemberIterator source) -> void
    {
    }

    auto scanImportModules(rapidjson::Value::MemberIterator source) -> void
    {
    }

    auto scanImportIncludesLocal(rapidjson::Value::MemberIterator source) -> void
    {
    }

    auto scanImportIncludesExternal(rapidjson::Value::MemberIterator source) -> void
    {
    }

    auto scanImportPartitions(rapidjson::Value::MemberIterator source) -> void
    {
    }

    auto scanSourceDependencies(rapidjson::Value::MemberIterator source) -> void
    {
        scanIncludesLocal();
        scanIncludesExternal();
        scanImportModules();
        scanImportIncludesLocal();
        scanImportIncludesExternal();
        scanImportPartitions();
    }

    BuildCache &mBuildCache;
};
}
