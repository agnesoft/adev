#ifdef _MSC_VER
export module abuild : build_graph;
export import : build_task;
import : build_cache;
#endif

namespace abuild
{
export class BuildGraph
{
public:
    explicit BuildGraph(BuildCache &cache) :
        mBuildCache{cache}
    {
        createLinkTasks();
        createCompileTasks();
    }

private:
    auto createCompileTask(Source *source) -> void
    {
        if (Module *mod = mBuildCache.cppModule(source))
        {
            mBuildCache.addBuildTask(mod->source, CompileModuleInterfaceTask{.source = source});
        }
        else if (ModulePartition *partition = mBuildCache.cppModulePartition(source))
        {
            mBuildCache.addBuildTask(partition->source, CompileModulePartitionTask{.source = source});
        }
        else
        {
            mBuildCache.addBuildTask(source, CompileSourceTask{.source = source});
        }
    }

    auto createCompileTasks() -> void
    {
        for (const std::unique_ptr<Source> &source : mBuildCache.sources())
        {
            createCompileTask(source.get());
        }
    }

    auto createLinkTask(Module *mod) -> void
    {
        mBuildCache.addBuildTask(mod, LinkModuleLibraryTask{.mod = mod});
    }

    auto createLinkTask(Project *project) -> void
    {
        if (hasSources(project))
        {
            mBuildCache.addBuildTask(project, createProjectLinkTask(project));
        }
    }

    auto createLinkTasks() -> void
    {
        createProjectLinkTasks();
        createModuleLinkTasks();
    }

    auto createModuleLinkTasks() -> void
    {
        for (const std::unique_ptr<Module> &mod : mBuildCache.modules())
        {
            createLinkTask(mod.get());
        }
    }

    auto createProjectLinkTasks() -> void
    {
        for (const std::unique_ptr<Project> &project : mBuildCache.projects())
        {
            createLinkTask(project.get());
        }
    }

    [[nodiscard]] static auto createProjectLinkTask(Project *project) -> BuildTask
    {
        if (project->type() == Project::Type::Executable)
        {
            return LinkExecutableTask{.project = project};
        }
        else
        {
            return LinkLibraryTask{.project = project};
        }
    }

    [[nodiscard]] auto hasSources(const Project *project) -> bool
    {
        for (const Source *source : project->sources())
        {
            if (!mBuildCache.cppModule(source) && !mBuildCache.cppModulePartition(source))
            {
                return true;
            }
        }

        return false;
    }

    BuildCache &mBuildCache;
};
}
