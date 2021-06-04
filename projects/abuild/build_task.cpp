#ifdef _MSC_VER
export module abuild : build_task;
export import<astl.hpp>;
#endif

namespace abuild
{
export class File;
export class Header;
export struct Module;
export struct ModulePartition;
export class Project;
export class Source;
export struct CompileHeaderUnitTask;
export struct CompileModuleInterfaceTask;
export struct CompileModulePartitionTask;
export struct CompileSourceTask;
export struct LinkExecutableTask;
export struct LinkLibraryTask;
export struct LinkModuleLibraryTask;

export struct PathHash
{
    [[nodiscard]] auto operator()(const std::filesystem::path &path) const noexcept -> std::size_t
    {
        return std::filesystem::hash_value(path);
    }
};

export using BuildTask = std::variant<
    CompileHeaderUnitTask,
    CompileModuleInterfaceTask,
    CompileModulePartitionTask,
    CompileSourceTask,
    LinkExecutableTask,
    LinkLibraryTask,
    LinkModuleLibraryTask>;

export struct CompileTask
{
    std::unordered_set<BuildTask *> inputTasks;
    std::unordered_set<std::filesystem::path, PathHash> includePaths;
};

export struct LinkTask
{
    std::unordered_set<BuildTask *> inputTasks;
};

export struct CompileHeaderUnitTask : CompileTask
{
    Header *header = nullptr;
};

export struct CompileModuleInterfaceTask : CompileTask
{
    Source *source = nullptr;
};

export struct CompileModulePartitionTask : CompileTask
{
    Source *source = nullptr;
};

export struct CompileSourceTask : CompileTask
{
    Source *source = nullptr;
};

export struct LinkExecutableTask : LinkTask
{
    Project *project = nullptr;
};

export struct LinkLibraryTask : LinkTask
{
    Project *project = nullptr;
};

export struct LinkModuleLibraryTask : LinkTask
{
    Module *mod = nullptr;
};
}
