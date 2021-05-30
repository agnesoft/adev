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
    static auto addInput(BuildTask *task, BuildTask *input) -> void
    {
        std::visit([&](auto &&value) {
            value.inputTasks.push_back(input);
        },
                   *task);
    }

    template<typename T>
    [[nodiscard]] auto buildTask(Source *source) -> BuildTask *
    {
        BuildTask *task = mBuildCache.buildTask(source);

        if (!task)
        {
            mBuildCache.addBuildTask(source, T{});
            task = mBuildCache.buildTask(source);
        }

        return task;
    }

    auto createCompileModuleInterfaceTask(Module *mod) -> void
    {
        BuildTask *task = buildTask<CompileModuleInterfaceTask>(mod->source);
        auto compileTask = &std::get<CompileModuleInterfaceTask>(*task);

        if (!compileTask->source)
        {
            compileTask->source = mod->source;
            addInput(mBuildCache.buildTask(mod), task);
        }
    }

    auto createCompileModulePartitionTask(ModulePartition *partition) -> void
    {
        BuildTask *task = buildTask<CompileModulePartitionTask>(partition->source);
        auto compileTask = &std::get<CompileModulePartitionTask>(*task);

        if (!compileTask->source)
        {
            compileTask->source = partition->source;
            addInput(mBuildCache.buildTask(partition->mod), task);
        }
    }

    auto createCompileSourceTask(Source *source)
    {
        BuildTask *task = buildTask<CompileSourceTask>(source);
        auto compileTask = &std::get<CompileSourceTask>(*task);

        if (!compileTask->source)
        {
            compileTask->source = source;
            addInput(mBuildCache.buildTask(source->project()), task);
        }
    }

    auto createCompileTask(Source *source) -> void
    {
        if (Module *mod = mBuildCache.cppModule(source))
        {
            createCompileModuleInterfaceTask(mod);
        }
        else if (ModulePartition *partition = mBuildCache.cppModulePartition(source))
        {
            createCompileModulePartitionTask(partition);
        }
        else
        {
            createCompileSourceTask(source);
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
