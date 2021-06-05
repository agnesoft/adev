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
    auto addDependency(CompileTask *compileTask, BuildTask *linkTask, const Dependency &dependency, File *base, std::unordered_set<File *> &includes) -> void
    {
        if (auto *dep = std::get_if<IncludeExternalHeaderDependency>(&dependency))
        {
            if (dep->header && !includes.contains(dep->header))
            {
                includes.insert(dep->header);
                addIncludePath(compileTask, base->path().parent_path(), includePath(dep->header->path(), dep->name));
                addDependencies(compileTask, linkTask, dep->header->dependencies(), dep->header, includes);
                addInput(linkTask, mBuildCache.buildTask(dep->header->project()));
            }
            return;
        }

        if (auto *dep = std::get_if<IncludeLocalHeaderDependency>(&dependency))
        {
            if (dep->header && !includes.contains(dep->header))
            {
                includes.insert(dep->header);
                addIncludePath(compileTask, base->path().parent_path(), includePath(dep->header->path(), dep->name));
                addDependencies(compileTask, linkTask, dep->header->dependencies(), dep->header, includes);
                addInput(linkTask, mBuildCache.buildTask(dep->header->project()));
            }
            return;
        }

        if (auto *dep = std::get_if<IncludeLocalSourceDependency>(&dependency))
        {
            if (dep->source && !includes.contains(dep->source))
            {
                includes.insert(dep->source);
                addIncludePath(compileTask, base->path().parent_path(), includePath(dep->source->path(), dep->name));
                addDependencies(compileTask, linkTask, dep->source->dependencies(), dep->source, includes);
            }
            return;
        }

        if (auto *dep = std::get_if<IncludeExternalSourceDependency>(&dependency))
        {
            if (dep->source && !includes.contains(dep->source))
            {
                includes.insert(dep->source);
                addIncludePath(compileTask, base->path().parent_path(), includePath(dep->source->path(), dep->name));
                addDependencies(compileTask, linkTask, dep->source->dependencies(), dep->source, includes);
            }
            return;
        }

        if (auto *dep = std::get_if<ImportExternalHeaderDependency>(&dependency))
        {
            addImportHeaderDependency(compileTask, linkTask, dep->header);
            return;
        }

        if (auto *dep = std::get_if<ImportLocalHeaderDependency>(&dependency))
        {
            addImportHeaderDependency(compileTask, linkTask, dep->header);
            return;
        }

        if (auto *dep = std::get_if<ImportModuleDependency>(&dependency))
        {
            addImportModuleDependency(compileTask, linkTask, dep->mod);
            return;
        }

        if (auto *dep = std::get_if<ImportModulePartitionDependency>(&dependency))
        {
            addImportModulePartitionDependency(compileTask, linkTask, dep->partition);
            return;
        }

        if (auto *dep = std::get_if<ImportSTLHeaderDependency>(&dependency))
        {
            compileTask->inputTasks.insert(createCompileSTLHeaderUnitTask(dep->name));
            return;
        }
    }

    auto addDependencies(CompileTask *compileTask, BuildTask *linkTask, const std::vector<Dependency> &dependencies, File *base, std::unordered_set<File *> &includes) -> void
    {
        for (const Dependency &dependency : dependencies)
        {
            addDependency(compileTask, linkTask, dependency, base, includes);
        }
    }

    auto addImportHeaderDependency(CompileTask *compileTask, BuildTask *linkTask, Header *header) -> void
    {
        if (header)
        {
            compileTask->inputTasks.insert(createCompileHeaderUnitTask(header));
            addInput(linkTask, mBuildCache.buildTask(header->project()));
        }
    }

    auto addImportModuleDependency(CompileTask *compileTask, BuildTask *linkTask, Module *mod) -> void
    {
        if (mod && mod->source)
        {
            compileTask->inputTasks.insert(createCompileModuleInterfaceTask(mod));
            addInput(linkTask, mBuildCache.buildTask(mod));
        }
    }

    auto addImportModulePartitionDependency(CompileTask *compileTask, [[maybe_unused]] BuildTask *linkTask, ModulePartition *partition) -> void
    {
        if (partition && partition->source)
        {
            compileTask->inputTasks.insert(createCompileModulePartitionTask(partition));
        }
    }

    auto addIncludePath(CompileTask *compileTask, const std::filesystem::path &base, const std::filesystem::path &include) -> void
    {
        if (include != base)
        {
            compileTask->includePaths.insert(include);
        }
    }

    static auto addInput(BuildTask *task, BuildTask *input) -> void
    {
        if (task && input && task != input)
        {
            std::visit([&](auto &&value) {
                value.inputTasks.insert(input);
            },
                       *task);
        }
    }

    template<typename T, typename E>
    [[nodiscard]] auto buildTask(E entity) -> BuildTask *
    {
        BuildTask *task = mBuildCache.buildTask(entity);

        if (!task)
        {
            task = mBuildCache.addBuildTask(entity, T{});
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
            std::unordered_set<File *> includes;
            addDependencies(compileTask, linkTask, mod->source->dependencies(), mod->source, includes);
        }

        return task;
    }

    [[nodiscard]] auto createCompileHeaderUnitTask(Header *header) -> BuildTask *
    {
        BuildTask *task = buildTask<CompileHeaderUnitTask>(header);
        auto compileTask = &std::get<CompileHeaderUnitTask>(*task);

        if (!compileTask->header)
        {
            compileTask->header = header;
            auto linkTask = createHeaderLinkTask(header->project());
            addInput(linkTask, task);
            std::unordered_set<File *> includes;
            addDependencies(compileTask, linkTask, header->dependencies(), header, includes);
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
            std::unordered_set<File *> includes;
            addDependencies(compileTask, linkTask, partition->source->dependencies(), partition->source, includes);
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
            std::unordered_set<File *> includes;
            addDependencies(compileTask, linkTask, source->dependencies(), source, includes);
        }

        return task;
    }

    [[nodiscard]] auto createCompileSTLHeaderUnitTask(const std::string &name) -> BuildTask *
    {
        BuildTask *task = buildTask<CompileSTLHeaderUnitTask>(name.c_str());
        auto compileTask = &std::get<CompileSTLHeaderUnitTask>(*task);

        if (compileTask->name.empty())
        {
            compileTask->name = name;
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

    [[nodiscard]] auto createHeaderLinkTask(Project *project) -> BuildTask *
    {
        BuildTask *task = buildTask<LinkLibraryTask>(project);
        auto linkTask = &std::get<LinkLibraryTask>(*task);

        if (!linkTask->project)
        {
            linkTask->project = project;
        }

        return task;
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

    [[nodiscard]] auto includePath(std::filesystem::path file, std::filesystem::path include) -> std::filesystem::path
    {
        file = file.parent_path();

        while (include.has_parent_path())
        {
            include = include.parent_path();
            file = file.parent_path();
        }

        return file;
    }

    BuildCache &mBuildCache;
};
}
