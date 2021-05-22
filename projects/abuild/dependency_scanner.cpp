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
        scan();
    }

private:
    auto scan() -> void
    {
        for (const std::unique_ptr<Source> &source : mBuildCache.sources())
        {
            scanFile(source.get());
        }

        for (const std::unique_ptr<Header> &header : mBuildCache.headers())
        {
            scanFile(header.get());
        }
    }

    auto scanDependency(Dependency *dependency, File *file) -> void
    {
        if (auto *value = std::get_if<IncludeExternalHeaderDependency>(dependency))
        {
            value->header = mBuildCache.header(value->name);
            return;
        }

        if (auto *value = std::get_if<IncludeLocalHeaderDependency>(dependency))
        {
            value->header = mBuildCache.header(value->name, file->path().parent_path());
            return;
        }

        if (auto *value = std::get_if<IncludeLocalSourceDependency>(dependency))
        {
            value->source = mBuildCache.source(value->name, file->path().parent_path());
            return;
        }

        if (auto *value = std::get_if<ImportExternalHeaderDependency>(dependency))
        {
            value->header = mBuildCache.header(value->name);
            return;
        }

        if (auto *value = std::get_if<ImportLocalHeaderDependency>(dependency))
        {
            value->header = mBuildCache.header(value->name, file->path().parent_path());
            return;
        }

        if (auto *value = std::get_if<ImportModuleDependency>(dependency))
        {
            value->mod = mBuildCache.moduleByName(value->name);
            return;
        }

        if (auto *value = std::get_if<ImportModulePartitionDependency>(dependency))
        {
            Module *mod = mBuildCache.moduleByFile(file);

            if (mod)
            {
                for (const std::unique_ptr<ModulePartition> &partition : mod->partitions)
                {
                    if (partition->name == value->name)
                    {
                        value->partition = partition.get();
                        break;
                    }
                }
            }

            return;
        }
    }

    auto scanFile(File *file) -> void
    {
        for (Dependency &dependency : file->dependencies())
        {
            scanDependency(&dependency, file);
        }
    }

    BuildCache &mBuildCache;
};
}
