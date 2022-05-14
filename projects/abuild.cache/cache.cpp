#ifndef __clang__
export module abuild.cache:cache;
export import :cache_data;
import :cache_index;
#endif

namespace abuild
{
export class Cache;

//! \private
auto read_cache(const std::filesystem::path &path, Cache &cache) -> void;
//! \private
auto write_cache(const CacheData &data) -> void;

//! The Cache class stores all the build
//! information.
//!
//! The Cache indexes all of its content for
//! querying. It is constructed with a path to the
//! cache file and uses YAML format. If the path
//! exists the cache is populated by the data from
//! the file. Upon destruction the cache data are
//! written back to the file.
//!
//! When loaded from a cache file all non-existent
//! files are skipped.
export class Cache
{
public:
    //! Constructs the `Cache` with file `path`.
    //! If the `path` exists it will read the data
    //! from it and populate the cache and build
    //! all the indexes.
    explicit Cache(std::filesystem::path path) :
        data{.filePath = std::move(path)}
    {
        if (std::filesystem::exists(this->data.filePath))
        {
            ::abuild::read_cache(this->data.filePath, *this);
        }
    }

    //! Deleted copy constructor.
    Cache(const Cache &other) = delete;

    //! Move constructor.
    Cache(Cache &&other) noexcept = default;

    //! Destructs the Cache and writes its data to
    //! the file passed in the constructor.
    ~Cache()
    {
        try
        {
            ::abuild::write_cache(this->data);
        }
        catch (...)
        {
        }
    }

    //! Returns the list of flags to be passed to all
    //! archiver calls.
    [[nodiscard]] auto archiver_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.archiverFlags;
    }

    //! Returns the list of flags to be passed to
    //! all compiler calls.
    [[nodiscard]] auto compiler_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.compilerFlags;
    }

    //! Returns the configuration name.
    [[nodiscard]] auto configuration_name() const noexcept -> const std::string &
    {
        return this->data.configurationName;
    }

    //! Returns the list of flags to be passed to
    //! every linker call.
    [[nodiscard]] auto linker_flags() const noexcept -> const std::vector<Flag> &
    {
        return this->data.linkerFlags;
    }

    //! Returns the list of defines.
    [[nodiscard]] auto defines() const noexcept -> const std::vector<DefineToken> &
    {
        return this->data.defines;
    }

    //! Adds header `file` to the Cache.
    auto add_header_file(const std::filesystem::path &path) -> HeaderFile *
    {
        HeaderFile *file = this->data.headers.emplace_back(std::make_unique<HeaderFile>()).get();
        file->path = path;
        this->index.insert(file);
        return file;
    }

    //! Adds header unit `file` to the Cache.
    auto add_header_unit(HeaderFile *file) -> HeaderUnit *
    {
        HeaderUnit *unit = this->data.headerUnits.emplace_back(std::make_unique<HeaderUnit>()).get();
        unit->headerFile = file;
        this->index.insert(unit);
        return unit;
    }

    //! Adds module `name` to the Cache.
    auto add_module(const std::string &name) -> Module *
    {
        Module *mod = this->data.modules.emplace_back(std::make_unique<Module>()).get();
        mod->name = name;
        this->index.insert(mod);
        return mod;
    }

    //! Adds module partition `name` to the cache.
    //! Note that you should associate it with a
    //! module.
    auto add_module_partition(const std::string &name) -> ModulePartition *
    {
        ModulePartition *partition = this->data.modulePartitions.emplace_back(std::make_unique<ModulePartition>()).get();
        partition->name = name;
        return partition;
    }

    //! Adds project `name` to the cache.
    auto add_project(std::string name) -> Project *
    {
        Project *proj = this->data.projects.emplace_back(std::make_unique<Project>()).get();
        proj->name = std::move(name);
        this->index.insert(proj);
        return proj;
    }

    //! Adds source `file` to the Cache.
    auto add_source_file(std::filesystem::path path) -> SourceFile *
    {
        SourceFile *file = this->data.sources.emplace_back(std::make_unique<SourceFile>()).get();
        file->path = std::move(path);
        this->index.insert(file);
        return file;
    }

    //! Returns path directory of the cache file.
    [[nodiscard]] auto build_root() const -> std::filesystem::path
    {
        return this->data.filePath.parent_path();
    }

    //! Finds the header with the exact `path` and
    //! returns it or `nullptr` if not found.
    [[nodiscard]] auto exact_header_file(const std::filesystem::path &path) const -> HeaderFile *
    {
        return this->index.exact_header_file(path);
    }

    //! Finds the source with the exact `path` and
    //! returns it or `nullptr` if not found.
    [[nodiscard]] auto exact_source_file(const std::filesystem::path &path) const -> SourceFile *
    {
        return this->index.exact_source_file(path);
    }

    //! Finds the first header matching the `path`
    //! such that the `path` is a subpath of the
    //! matched file. E.g. searching for
    //! `include/my_header.hpp` could match
    //! `project1/include/my_header.hpp`,
    //! `project2/include/my_header.hpp` etc. If
    //! there are multiple matching files their
    //! order is unspecified. If no file is
    //! matched returns `nullptr`.
    [[nodiscard]] auto header_file(const std::filesystem::path &path) const -> HeaderFile *
    {
        return this->index.header_file(path);
    }

    //! Finds the header unit associated with the
    //! `file` or `nullptr` if not found.
    [[nodiscard]] auto header_unit(HeaderFile *file) const -> HeaderUnit *
    {
        return this->index.header_unit(file);
    }

    //! Finds the module `name` or `nullptr` if
    //! not found.
    [[nodiscard]] auto module_(const std::string &name) const -> Module * // NOLINT(readability-identifier-naming)
    {
        return this->index.module_(name);
    }

    //! Find the project with `name`. If no such
    //! project is found returns `nullptr`.
    [[nodiscard]] auto project(const std::string &name) const -> Project *
    {
        return this->index.project(name);
    }

    //! Returns the source (project) directory
    //! root.
    [[nodiscard]] auto project_root() const noexcept -> const std::filesystem::path &
    {
        return this->data.projectRoot;
    }

    //! Returns the list of projects.
    [[nodiscard]] auto projects() const noexcept -> const std::vector<std::unique_ptr<Project>> &
    {
        return this->data.projects;
    }

    //! Returns the internal read-only Settings
    //! struct.
    [[nodiscard]] auto settings() const noexcept -> const Settings &
    {
        return this->data.settings;
    }

    //! Sets the flags to be passed to the
    //! archiver calls.
    auto set_archiver_flags(std::vector<Flag> flags) noexcept -> void
    {
        this->data.archiverFlags = std::move(flags);
    }

    //! Returns the configuration name.
    auto set_configuration_name(const std::string &name) -> void
    {
        this->data.configurationName = name;
    }

    //! Sets the defines.
    auto set_defines(std::vector<DefineToken> values) noexcept -> void
    {
        this->data.defines = std::move(values);
    }

    //! Sets the flags to be passed to the
    //! compiler calls.
    auto set_compiler_flags(std::vector<Flag> flags) noexcept -> void
    {
        this->data.compilerFlags = std::move(flags);
    }

    //! Sets the flags to be passed to the linker
    //! calls.
    auto set_linker_flags(std::vector<Flag> flags) noexcept -> void
    {
        this->data.linkerFlags = std::move(flags);
    }

    //! Sets the source (project) root.
    auto set_project_root(std::filesystem::path path) noexcept -> void
    {
        this->data.projectRoot = std::move(path);
    }

    //! Sets the settings.
    auto set_settings(Settings settings) -> void
    {
        this->data.settings = std::move(settings);
    }

    //! Sets the toolchain.
    auto set_toolchain(Toolchain toolchain) -> void
    {
        this->data.toolchain = std::move(toolchain);
    }

    //! Returns the toolchain.
    [[nodiscard]] auto toolchain() const noexcept -> const Toolchain &
    {
        return this->data.toolchain;
    }

    //! Finds the first source matching the `path`
    //! such that the `path` is a subpath of the
    //! matched file. E.g. searching for
    //! `main.cpp` could match
    //! `project1/main.cpp`, `project2/main.cpp`
    //! etc. If there are multiple matching files
    //! their order is unspecified. If no file is
    //! matched returns `nullptr`.
    [[nodiscard]] auto source_file(const std::filesystem::path &path) const -> SourceFile *
    {
        return this->index.source_file(path);
    }

    //! Deleted copy assignment.
    auto operator=(const Cache &other) -> Cache & = delete;

    //! Move assignment.
    auto operator=(Cache &&other) noexcept -> Cache & = default;

private:
    CacheData data;
    CacheIndex index;
};
}