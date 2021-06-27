#ifdef _MSC_VER
export module abuild : build_cache;
export import : error;
export import : warning;
export import : toolchain;
export import : sdk;
export import : build_cache_index;
export import : abuild_override;
#endif

namespace abuild
{
export class BuildCache
{
public:
    BuildCache() :
        BuildCache(std::filesystem::current_path())
    {
    }

    BuildCache(const std::filesystem::path &projectRoot) :
        mData{.projectRoot{projectRoot}, .dataOverride{projectRoot}}
    {
        mData.dataOverride.applyOverride(&mData.settings);
    }

    auto addBuildTask(const void *entity, BuildTask buildTask) -> BuildTask *
    {
        BuildTask *task = mData.buildTasks.emplace_back(std::make_unique<BuildTask>(std::move(buildTask))).get();
        mIndex.addBuildTask(entity, task);
        return task;
    }

    auto addBuildTask(const char *name, BuildTask buildTask) -> BuildTask *
    {
        BuildTask *task = mData.buildTasks.emplace_back(std::make_unique<BuildTask>(std::move(buildTask))).get();
        mIndex.addBuildTask(name, task);
        return task;
    }

    auto addError(Error error) -> void
    {
        mData.errors.push_back(std::move(error));
    }

    auto addHeader(const std::filesystem::path &path, const std::string &projectName) -> Header *
    {
        Project *proj = getProject(projectName);
        Header *header = mData.headers.emplace_back(std::make_unique<Header>(path, proj)).get();
        proj->addHeader(header);
        mIndex.addHeader(path.filename().string(), header);
        return header;
    }

    auto addModuleInterface(const std::string &moduleName, ModuleVisibility visibility, Source *source) -> Module *
    {
        Module *mod = getCppModule(moduleName);
        mod->source = source;
        mod->visibility = visibility;
        mIndex.addModuleFile(source, mod);
        return mod;
    }

    auto addModulePartition(const std::string &moduleName, std::string partitionName, ModuleVisibility visibility, Source *source) -> ModulePartition *
    {
        Module *mod = getCppModule(moduleName);
        ModulePartition *partition = mData.modulePartitions.emplace_back(std::make_unique<ModulePartition>()).get();
        mod->partitions.push_back(partition);
        partition->name = std::move(partitionName);
        partition->visibility = visibility;
        partition->source = source;
        partition->mod = mod;
        mIndex.addModulePartitionFile(source, partition);
        return partition;
    }

    auto addSource(const std::filesystem::path &path, const std::string &projectName) -> Source *
    {
        Project *proj = getProject(projectName);
        Source *source = mData.sources.emplace_back(std::make_unique<Source>(path, proj)).get();
        proj->addSource(source);
        mIndex.addSource(path.filename().string(), source);
        return source;
    }

    auto addToolchain(Toolchain toolchain) -> Toolchain *
    {
        return mData.toolchains.emplace_back(std::make_unique<Toolchain>(std::move(toolchain))).get();
    }

    auto addWarning(Warning warning) -> void
    {
        mData.warnings.push_back(std::move(warning));
    }

    [[nodiscard]] auto buildTasks() const noexcept -> const std::vector<std::unique_ptr<BuildTask>> &
    {
        return mData.buildTasks;
    }

    [[nodiscard]] auto buildTask(const void *entity) const -> BuildTask *
    {
        return mIndex.buildTask(entity);
    }

    [[nodiscard]] auto buildTask(const char *name) const -> BuildTask *
    {
        return mIndex.buildTask(name);
    }

    [[nodiscard]] auto cppModule(const File *file) const -> Module *
    {
        return mIndex.cppModule(file);
    }

    [[nodiscard]] auto cppModule(const std::string &name) const -> Module *
    {
        return mIndex.cppModule(name);
    }

    [[nodiscard]] auto cppModulePartition(const File *file) const -> ModulePartition *
    {
        return mIndex.cppModulePartition(file);
    }

    [[nodiscard]] auto errors() const noexcept -> const std::vector<Error> &
    {
        return mData.errors;
    }

    [[nodiscard]] auto header(const std::filesystem::path &file) const -> Header *
    {
        return header(file, {});
    }

    [[nodiscard]] auto header(const std::filesystem::path &file, const std::filesystem::path &hint) const -> Header *
    {
        return mIndex.header(file, hint);
    }

    [[nodiscard]] auto headers() const noexcept -> const std::vector<std::unique_ptr<Header>> &
    {
        return mData.headers;
    }

    [[nodiscard]] auto modules() const noexcept -> const std::vector<std::unique_ptr<Module>> &
    {
        return mData.modules;
    }

    [[nodiscard]] auto project(const std::string &name) const -> Project *
    {
        return mIndex.project(name);
    }

    [[nodiscard]] auto projectRoot() const noexcept -> const std::filesystem::path &
    {
        return mData.projectRoot;
    }

    [[nodiscard]] auto projects() const noexcept -> const std::vector<std::unique_ptr<Project>> &
    {
        return mData.projects;
    }

    [[nodiscard]] auto settings() const noexcept -> const Settings &
    {
        return mData.settings;
    }

    [[nodiscard]] auto source(const std::filesystem::path &file) const -> Source *
    {
        return source(file, {});
    }

    [[nodiscard]] auto source(const std::filesystem::path &file, const std::filesystem::path &hint) const -> Source *
    {
        return mIndex.source(file, hint);
    }

    [[nodiscard]] auto sources() const noexcept -> const std::vector<std::unique_ptr<Source>> &
    {
        return mData.sources;
    }

    [[nodiscard]] auto toolchain(const std::string &name) const -> Toolchain *
    {
        for (const std::unique_ptr<Toolchain> &toolchain : mData.toolchains)
        {
            if (toolchain->name.starts_with(name))
            {
                return toolchain.get();
            }
        }

        return nullptr;
    }

    [[nodiscard]] auto toolchains() const noexcept -> const std::vector<std::unique_ptr<Toolchain>> &
    {
        return mData.toolchains;
    }

    [[nodiscard]] auto warnings() const noexcept -> const std::vector<Warning> &
    {
        return mData.warnings;
    }

private:
    struct Data
    {
        std::vector<std::unique_ptr<Project>> projects;
        std::vector<std::unique_ptr<Source>> sources;
        std::vector<std::unique_ptr<Header>> headers;
        std::vector<std::unique_ptr<Module>> modules;
        std::vector<std::unique_ptr<ModulePartition>> modulePartitions;
        std::vector<std::unique_ptr<BuildTask>> buildTasks;
        std::vector<std::unique_ptr<Toolchain>> toolchains;
        std::vector<Error> errors;
        std::vector<Warning> warnings;
        std::filesystem::path projectRoot;
        Settings settings;
        Override dataOverride;
    };

    [[nodiscard]] auto getCppModule(const std::string &name) -> Module *
    {
        Module *mod = mIndex.cppModule(name);

        if (!mod)
        {
            mod = mData.modules.emplace_back(std::make_unique<Module>()).get();
            mod->name = name;
            mIndex.addModule(name, mod);
        }

        return mod;
    }

    [[nodiscard]] auto getProject(const std::string &name) -> Project *
    {
        Project *proj = mIndex.project(name);

        if (!proj)
        {
            proj = mData.projects.emplace_back(std::make_unique<Project>(name)).get();
            mIndex.addProject(name, proj);
        }

        return proj;
    }

    Data mData;
    BuildCacheIndex mIndex;
};
}
