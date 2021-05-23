#ifdef _MSC_VER
export module abuild : build_cache;
export import : project;
export import : header;
export import : source;
export import : cpp_module;
export import : error;
export import : warning;
#endif

namespace abuild
{
export class BuildCache
{
public:
    auto addError(Error error) -> void
    {
        mErrors.push_back(std::move(error));
    }

    auto addHeader(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        Header *header = mHeaders.emplace_back(std::make_unique<Header>(path, project(projectName))).get();
        mHeaderIndex.insert({path.filename().string(), header});
    }

    auto addModuleInterface(const std::string &moduleName, ModuleVisibility visibility, Source *source) -> void
    {
        Module *mod = cppmodule(moduleName);
        mod->source = source;
        mod->visibility = visibility;
        mModuleFileIndex.insert({source, mod});
    }

    auto addModulePartition(const std::string &moduleName, std::string partitionName, ModuleVisibility visibility, Source *source) -> void
    {
        Module *mod = cppmodule(moduleName);
        ModulePartition *partition = mod->partitions.emplace_back(std::make_unique<ModulePartition>()).get();
        partition->name = std::move(partitionName);
        partition->visibility = visibility;
        partition->source = source;
        partition->mod = mod;
        mModulePartitionsFileIndex.insert({source, partition});
    }

    auto addSource(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        Source *source = mSources.emplace_back(std::make_unique<Source>(path, project(projectName))).get();
        mSourceIndex.insert({path.filename().string(), source});
    }

    auto addWarning(Warning warning) -> void
    {
        mWarnings.push_back(std::move(warning));
    }

    [[nodiscard]] auto errors() const noexcept -> const std::vector<Error> &
    {
        return mErrors;
    }

    [[nodiscard]] auto header(const std::filesystem::path &file) const -> Header *
    {
        return header(file, {});
    }

    [[nodiscard]] auto header(const std::filesystem::path &file, const std::filesystem::path &hint) const -> Header *
    {
        using It = std::unordered_multimap<std::string, Header *>::const_iterator;
        std::pair<It, It> range = mHeaderIndex.equal_range(file.filename().string());

        if (!hint.empty())
        {
            const std::filesystem::path hintedPath = hint / file;

            for (It it = range.first; it != range.second; ++it)
            {
                if (it->second->path() == hintedPath)
                {
                    return it->second;
                }
            }
        }

        for (It it = range.first; it != range.second; ++it)
        {
            if (isSame(it->second->path(), file))
            {
                return it->second;
            }
        }

        return nullptr;
    }

    [[nodiscard]] auto headers() const noexcept -> const std::vector<std::unique_ptr<Header>> &
    {
        return mHeaders;
    }

    [[nodiscard]] auto moduleByFile(const File *file) const -> Module *
    {
        std::unordered_map<const File *, Module *>::const_iterator it = mModuleFileIndex.find(file);

        if (it != mModuleFileIndex.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    [[nodiscard]] auto moduleByName(const std::string &name) const -> Module *
    {
        std::unordered_map<std::string, Module *>::const_iterator it = mModuleIndex.find(name);

        if (it != mModuleIndex.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    [[nodiscard]] auto modulePartitionByFile(const File *file) const -> ModulePartition *
    {
        std::unordered_map<const File *, ModulePartition *>::const_iterator it = mModulePartitionsFileIndex.find(file);

        if (it != mModulePartitionsFileIndex.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    [[nodiscard]] auto modules() const noexcept -> const std::vector<std::unique_ptr<Module>> &
    {
        return mModules;
    }

    [[nodiscard]] auto projects() const noexcept -> const std::vector<std::unique_ptr<Project>> &
    {
        return mProjects;
    }

    [[nodiscard]] auto source(const std::filesystem::path &file, const std::filesystem::path &hint) const -> Source *
    {
        using It = std::unordered_multimap<std::string, Source *>::const_iterator;
        std::pair<It, It> range = mSourceIndex.equal_range(file.filename().string());

        if (!hint.empty())
        {
            const std::filesystem::path hintedPath = hint / file;

            for (It it = range.first; it != range.second; ++it)
            {
                if (it->second->path() == hintedPath)
                {
                    return it->second;
                }
            }
        }

        for (It it = range.first; it != range.second; ++it)
        {
            if (isSame(it->second->path(), file))
            {
                return it->second;
            }
        }

        return nullptr;
    }

    [[nodiscard]] auto source(const std::filesystem::path &file) const -> Source *
    {
        return source(file, {});
    }

    [[nodiscard]] auto sources() const noexcept -> const std::vector<std::unique_ptr<Source>> &
    {
        return mSources;
    }

    [[nodiscard]] auto warnings() const noexcept -> const std::vector<Warning> &
    {
        return mWarnings;
    }

private:
    [[nodiscard]] auto cppmodule(const std::string &name) -> Module *
    {
        std::unordered_map<std::string, Module *>::iterator it = mModuleIndex.find(name);

        if (it == mModuleIndex.end())
        {
            Module *m = mModules.emplace_back(std::make_unique<Module>()).get();
            m->name = name;
            it = mModuleIndex.insert({name, m}).first;
        }

        return it->second;
    }

    [[nodiscard]] static auto isSame(std::filesystem::path left, std::filesystem::path right) -> bool
    {
        while (right.has_parent_path())
        {
            left = left.parent_path();
            right = right.parent_path();

            if (left.filename() != right.filename())
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] auto project(const std::string &name) -> Project *
    {
        std::unordered_map<std::string, Project *>::iterator it = mProjectIndex.find(name);

        if (it == mProjectIndex.end())
        {
            Project *p = mProjects.emplace_back(std::make_unique<Project>(name)).get();
            it = mProjectIndex.insert({name, p}).first;
        }

        return it->second;
    }

    std::vector<std::unique_ptr<Project>> mProjects;
    std::vector<std::unique_ptr<Source>> mSources;
    std::vector<std::unique_ptr<Header>> mHeaders;
    std::vector<std::unique_ptr<Module>> mModules;
    std::vector<Error> mErrors;
    std::vector<Warning> mWarnings;
    std::unordered_map<std::string, Project *> mProjectIndex;
    std::unordered_map<std::string, Module *> mModuleIndex;
    std::unordered_multimap<std::string, Source *> mSourceIndex;
    std::unordered_multimap<std::string, Header *> mHeaderIndex;
    std::unordered_map<const File *, Module *> mModuleFileIndex;
    std::unordered_map<const File *, ModulePartition *> mModulePartitionsFileIndex;
};
}
