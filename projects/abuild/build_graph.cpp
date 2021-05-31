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
    auto addDependency(CompileTask *compileTask, LinkTask *linkTask, const Dependency &dependency) -> void
    {
        if (auto *dep = std::get_if<IncludeExternalHeaderDependency>(&dependency))
        {
            if (dep->header)
            {
                addDependencies(compileTask, linkTask, dep->header->dependencies());

                BuildTask *link = mBuildCache.buildTask(dep->header->project());

                if (link)
                {
                    linkTask->inputTasks.insert(link);
                }
            }

            return;
        }

        if (auto *dep = std::get_if<ImportModuleDependency>(&dependency))
        {
            if (dep->mod && dep->mod->source)
            {
                compileTask->inputTasks.insert(createCompileModuleInterfaceTask(dep->mod));

                BuildTask *link = mBuildCache.buildTask(dep->mod);

                if (link)
                {
                    linkTask->inputTasks.insert(link);
                }
            }
        }

        if (auto *dep = std::get_if<ImportModulePartitionDependency>(&dependency))
        {
            if (dep->partition && dep->partition->source)
            {
                compileTask->inputTasks.insert(createCompileModulePartitionTask(dep->partition));
            }

            return;
        }
    }

    auto addDependencies(CompileTask *compileTask, LinkTask *linkTask, const std::vector<Dependency> &dependencies) -> void
    {
        for (const Dependency &dependency : dependencies)
        {
            addDependency(compileTask, linkTask, dependency);
        }
    }

    static auto addInput(BuildTask *task, BuildTask *input) -> void
    {
        std::visit([&](auto &&value) {
            value.inputTasks.insert(input);
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

    [[nodiscard]] auto createCompileModuleInterfaceTask(Module *mod) -> BuildTask *
    {
        BuildTask *task = buildTask<CompileModuleInterfaceTask>(mod->source);
        auto compileTask = &std::get<CompileModuleInterfaceTask>(*task);

        if (!compileTask->source)
        {
            compileTask->source = mod->source;
            auto linkTask = mBuildCache.buildTask(mod);
            addInput(linkTask, task);
            addDependencies(compileTask, &std::get<LinkModuleLibraryTask>(*linkTask), mod->source->dependencies());
        }

        return task;
    }

    [[nodiscard]] auto createCompileModulePartitionTask(ModulePartition *partition) -> BuildTask *
    {
        BuildTask *task = buildTask<CompileModulePartitionTask>(partition->source);
        auto compileTask = &std::get<CompileModulePartitionTask>(*task);

        if (!compileTask->source)
        {
            compileTask->source = partition->source;
            auto linkTask = mBuildCache.buildTask(partition->mod);
            addInput(linkTask, task);
            addDependencies(compileTask, &std::get<LinkModuleLibraryTask>(*linkTask), partition->source->dependencies());
        }

        return task;
    }

    [[nodiscard]] auto createCompileSourceTask(Source *source) -> BuildTask *
    {
        BuildTask *task = buildTask<CompileSourceTask>(source);
        auto compileTask = &std::get<CompileSourceTask>(*task);

        if (!compileTask->source)
        {
            compileTask->source = source;
            auto linkTask = mBuildCache.buildTask(source->project());
            addInput(linkTask, task);
            std::visit([&](auto &&value) {
                using T = std::remove_pointer_t<std::decay_t<decltype(value)>>;

                if constexpr (std::is_base_of_v<LinkTask, T>)
                {
                    addDependencies(compileTask, &value, source->dependencies());
                }
            },
                       *linkTask);
        }

        return task;
    }

    auto createCompileTask(Source *source) -> BuildTask *
    {
        if (Module *mod = mBuildCache.cppModule(source))
        {
            return createCompileModuleInterfaceTask(mod);
        }
        else if (ModulePartition *partition = mBuildCache.cppModulePartition(source))
        {
            return createCompileModulePartitionTask(partition);
        }
        else
        {
            return createCompileSourceTask(source);
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
