#ifdef _MSC_VER
export module abuild : build_cache_index;
export import : project;
export import : header;
export import : source;
export import : cpp_module;
export import : build_task;
#endif

namespace abuild
{
class BuildCacheIndex
{
public:
    auto addBuildTask(const void *entity, BuildTask *task) -> void
    {
        mBuildTaskIndex.insert({entity, task});
    }

    auto addHeader(const std::string &name, Header *header) -> void
    {
        mHeaderIndex.insert({name, header});
    }

    auto addModule(const std::string &name, Module *mod) -> void
    {
        mModuleIndex.insert({name, mod});
    }

    auto addModuleFile(const File *file, Module *mod) -> void
    {
        mModuleFileIndex.insert({file, mod});
    }

    auto addModulePartitionFile(const File *file, ModulePartition *partition) -> void
    {
        mModulePartitionsFileIndex.insert({file, partition});
    }

    auto addProject(const std::string &name, Project *proj) -> void
    {
        mProjectIndex.insert({name, proj});
    }

    auto addSource(const std::string &name, Source *source) -> void
    {
        mSourceIndex.insert({name, source});
    }

    [[nodiscard]] auto buildTask(const void *entity) const -> BuildTask *
    {
        std::unordered_map<const void *, BuildTask *>::const_iterator it = mBuildTaskIndex.find(entity);

        if (it != mBuildTaskIndex.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    [[nodiscard]] auto header(const std::filesystem::path &file, const std::filesystem::path &hint) const -> Header *
    {
        using It = std::unordered_multimap<std::string, Header *>::const_iterator;
        std::pair<It, It> range = mHeaderIndex.equal_range(file.filename().string());

        if (!hint.empty())
        {
            const std::filesystem::path hintedPath = (hint / file).lexically_normal();

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

    [[nodiscard]] auto cppModule(const File *file) const -> Module *
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

    [[nodiscard]] auto cppModule(const std::string &name) const -> Module *
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

    [[nodiscard]] auto cppModulePartition(const File *file) const -> ModulePartition *
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

    [[nodiscard]] auto project(const std::string &name) const -> Project *
    {
        std::unordered_map<std::string, Project *>::const_iterator it = mProjectIndex.find(name);

        if (it != mProjectIndex.end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    [[nodiscard]] auto source(const std::filesystem::path &file, const std::filesystem::path &hint) const -> Source *
    {
        using It = std::unordered_multimap<std::string, Source *>::const_iterator;
        std::pair<It, It> range = mSourceIndex.equal_range(file.filename().string());

        if (!hint.empty())
        {
            const std::filesystem::path hintedPath = (hint / file).lexically_normal();

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

private:
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

    std::unordered_map<std::string, Project *> mProjectIndex;
    std::unordered_map<std::string, Module *> mModuleIndex;
    std::unordered_multimap<std::string, Source *> mSourceIndex;
    std::unordered_multimap<std::string, Header *> mHeaderIndex;
    std::unordered_map<const File *, Module *> mModuleFileIndex;
    std::unordered_map<const File *, ModulePartition *> mModulePartitionsFileIndex;
    std::unordered_map<const void *, BuildTask *> mBuildTaskIndex;
};
}
