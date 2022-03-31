#ifndef __clang__
export module abuild.cache:cache_impl;
export import :cache_data;
import :cache_index;
#endif

namespace abuild
{
export class Cache;

//! \private
auto read_cache(const std::filesystem::path &path, Cache &cache) -> void;
//! \private
auto write_cache(const std::filesystem::path &path, const CacheData &data) -> void;

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
//! The Cache YAML schema:
//!
//! ```
//! headers:
//!   path/to/header1:
//!     project: my_project
//!     timestamp: 123
//!     tokens:
//!       - #if defined(SOME_MACRO)
//!       - #define MY_MACRO
//!       - #endif
//! sources:
//!   path/to/source:
//!     project: my_project
//!     timestamp: 456
//!     tokens:
//! ```
//!
//! NOTE: When loaded from a cache file all
//! non-existent files are skipped.
export class Cache
{
public:
    //! Constructs the `Cache` with `path`. If the
    //! `path` exists it will read the data from it
    //! and populate the cache and build all the
    //! indexes.
    explicit Cache(std::filesystem::path path) :
        filePath{std::move(path)}
    {
        if (std::filesystem::exists(this->filePath))
        {
            ::abuild::read_cache(this->filePath, *this);
        }
    }

    //! Deleted copy constructor.
    Cache(const Cache &other) = delete;

    //! Move constructor.
    Cache(Cache &&other) noexcept = default;

    //! Destructs the Cache and writes its data to
    //! the file.
    ~Cache()
    {
        try
        {
            ::abuild::write_cache(this->filePath, this->data);
        }
        catch (...)
        {
        }
    }

    //! Adds header `file` to the Cache and
    //! associates it with `projectName` project.
    //! If the project does not exist it is
    //! created. Returns the pointer to the
    //! inserted HeaderFile.
    auto add_header_file(File headerFile, const std::string &projectName) -> HeaderFile *
    {
        Project *proj = this->get_project(projectName);
        HeaderFile *file = this->data.headers.emplace_back(std::make_unique<HeaderFile>(HeaderFile{File{std::move(headerFile)}, proj, {}})).get();
        proj->headers.push_back(file);
        this->index.insert(file);
        return file;
    }

    //! Adds source `file` to the Cache and
    //! associates it with `projectName` project.
    //! If the project does not exist it is
    //! created. Returns the pointer to the
    //! inserted SourceFile.
    auto add_source_file(File sourceFile, const std::string &projectName) -> SourceFile *
    {
        Project *proj = this->get_project(projectName);
        SourceFile *file = this->data.sources.emplace_back(std::make_unique<SourceFile>(SourceFile{File{std::move(sourceFile)}, proj, {}})).get();
        proj->sources.push_back(file);
        this->index.insert(file);
        return file;
    }

    //! Finds the header with the exact `path` and
    //! returns it or nullptr if not found.
    [[nodiscard]] auto exact_header_file(const std::filesystem::path &path) const -> HeaderFile *
    {
        return this->index.exact_header_file(path);
    }

    //! Finds the source with the exact `path` and
    //! returns it or nullptr if not found.
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

    //! Returns the list of all header files.
    [[nodiscard]] auto header_files() const noexcept -> const std::vector<std::unique_ptr<HeaderFile>> &
    {
        return this->data.headers;
    }

    //! Find the project with `name`. If no such
    //! project is found returns `nullptr`.
    [[nodiscard]] auto project(const std::string &name) const -> Project *
    {
        return this->index.project(name);
    }

    //! Returns the list of projects.
    [[nodiscard]] auto projects() const noexcept -> const std::vector<std::unique_ptr<Project>> &
    {
        return this->data.projects;
    }

    //! Returns root of the project which is the
    //! directory containing the cache file by
    //! default.
    [[nodiscard]] auto project_root() const -> std::filesystem::path
    {
        return this->filePath.parent_path();
    }

    //! Returns the internal read-only Settings
    //! struct.
    [[nodiscard]] auto settings() const noexcept -> const Settings &
    {
        return this->data.settings;
    }

    //! Finds the first source matching the `path`
    //! such that the `path` is a subpath of the
    //! matched file. E.g. searching for
    //! `main.cpp` could match
    //! `project1/main.cpp`,
    //! `project2/main.cpp` etc. If
    //! there are multiple matching files their
    //! order is unspecified. If no file is
    //! matched returns `nullptr`.
    [[nodiscard]] auto source_file(const std::filesystem::path &path) const -> SourceFile *
    {
        return this->index.source_file(path);
    }

    //! Returns the list of all source files.
    [[nodiscard]] auto source_files() const noexcept -> const std::vector<std::unique_ptr<SourceFile>> &
    {
        return this->data.sources;
    }

    //! Deleted copy assignment.
    auto operator=(const Cache &other) -> Cache & = delete;

    //! Move assignment.
    auto operator=(Cache &&other) noexcept -> Cache & = default;

private:
    auto get_project(const std::string &name) -> Project *
    {
        Project *proj = this->project(name);

        if (proj == nullptr)
        {
            proj = this->data.projects.emplace_back(std::make_unique<Project>(Project{.name = name})).get();
            this->index.insert(proj);
        }

        return proj;
    }

    std::filesystem::path filePath;
    CacheData data;
    CacheIndex index;
};
}