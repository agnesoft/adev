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
    [[nodiscard]] auto moduleFromFile(File *file) -> Module *
    {
        Module *mod = mBuildCache.moduleByFile(file);

        if (!mod)
        {
            ModulePartition *partition = mBuildCache.modulePartitionByFile(file);

            if (partition)
            {
                mod = partition->mod;
            }
        }

        return mod;
    }

    [[nodiscard]] auto modulePartition(Module *mod, const std::string &name) -> ModulePartition *
    {
        for (const std::unique_ptr<ModulePartition> &partition : mod->partitions)
        {
            if (partition->name == name)
            {
                return partition.get();
            }
        }

        return nullptr;
    }

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
            validateHeader(value->header, value->name, file);
            return;
        }

        if (auto *value = std::get_if<IncludeLocalHeaderDependency>(dependency))
        {
            value->header = mBuildCache.header(value->name, file->path().parent_path());
            validateHeader(value->header, value->name, file);
            return;
        }

        if (auto *value = std::get_if<IncludeLocalSourceDependency>(dependency))
        {
            value->source = mBuildCache.source(value->name, file->path().parent_path());
            validateSource(value->source, value->name, file);
            return;
        }

        if (auto *value = std::get_if<ImportExternalHeaderDependency>(dependency))
        {
            value->header = mBuildCache.header(value->name);
            validateHeader(value->header, value->name, file);
            return;
        }

        if (auto *value = std::get_if<ImportLocalHeaderDependency>(dependency))
        {
            value->header = mBuildCache.header(value->name, file->path().parent_path());
            validateHeader(value->header, value->name, file);
            return;
        }

        if (auto *value = std::get_if<ImportModuleDependency>(dependency))
        {
            value->mod = mBuildCache.moduleByName(value->name);
            validateModule(value->mod, value->name, file);
            return;
        }

        if (auto *value = std::get_if<ImportModulePartitionDependency>(dependency))
        {
            Module *mod = moduleFromFile(file);

            if (mod)
            {
                value->partition = modulePartition(mod, value->name);
                validateModulePartition(value->partition, value->name, mod->name, file);
            }
            else
            {
                mBuildCache.addWarning(Warning{.component = COMPONENT, .what = "Module of module partition '" + value->name + "' not found. (" + file->path().string() + ')'});
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

    auto validateHeader(Header *header, std::string name, File *file) -> void
    {
        if (!header)
        {
            mBuildCache.addWarning(Warning{.component = COMPONENT, .what = "Header '" + name + "' not found. (" + file->path().string() + ')'});
        }
    }

    auto validateModule(Module *mod, std::string name, File *file) -> void
    {
        if (!mod)
        {
            mBuildCache.addWarning(Warning{.component = COMPONENT, .what = "Module '" + name + "' not found. (" + file->path().string() + ')'});
        }
    }

    auto validateModulePartition(ModulePartition *partition, std::string name, std::string moduleName, File *file) -> void
    {
        if (!partition)
        {
            mBuildCache.addWarning(Warning{.component = COMPONENT, .what = "Module partition '" + name + "' not found in module '" + moduleName + "'. (" + file->path().string() + ')'});
        }
    }

    auto validateSource(Source *source, std::string name, File *file) -> void
    {
        if (!source)
        {
            mBuildCache.addWarning(Warning{.component = COMPONENT, .what = "Source '" + name + "' not found. (" + file->path().string() + ')'});
        }
    }

    BuildCache &mBuildCache;
    static constexpr char COMPONENT[] = "DependencyScanner";
};
}
