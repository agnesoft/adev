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
export struct IncludeTask;
export struct LinkExecutableTask;
export struct LinkLibraryTask;
export struct LinkModuleLibraryTask;

export using BuildTask = std::variant<
    CompileHeaderUnitTask,
    CompileModuleInterfaceTask,
    CompileModulePartitionTask,
    CompileSourceTask,
    IncludeTask,
    LinkExecutableTask,
    LinkLibraryTask,
    LinkModuleLibraryTask>;

export struct CompileTask
{
    std::vector<BuildTask *> inputTasks;
};

export struct LinkTask
{
    std::vector<BuildTask *> inputTasks;
};

export struct IncludeTask : CompileTask
{
    File *file = nullptr;
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
