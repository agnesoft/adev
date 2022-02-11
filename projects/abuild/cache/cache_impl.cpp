#ifndef __clang__
export module abuild.cache : cache_impl;
import : cache_data;
import : cache_index;
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
//! cache file in YAML format. If the path exists
//! the cache is populated by the data from the
//! file. Upon destruction the cache data are
//! written back to the file.
export class Cache
{
    [[nodiscard]] auto insert_file(std::filesystem::path path, auto &files)
    {
        using T = typename std::remove_cvref_t<decltype(files)>::value_type::element_type;
        T *file = files.emplace_back(std::make_unique<T>(T{File{.path = std::move(path)}})).get();
        this->index.insert(file);
        return file;
    }

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

    //! Adds header file identified by `path` to
    //! the Cache. Returns the pointer to the
    //! inserted HeaderFile.
    auto add_header_file(std::filesystem::path path) -> HeaderFile *
    {
        return Cache::insert_file(std::move(path), this->data.headers);
    }

    //! Adds new project named `name` to the
    //! project. Returns a pointer to the added
    //! Project.
    auto add_project(std::string name) -> Project *
    {
        Project *project = this->data.projects.emplace_back(std::make_unique<Project>(Project{.name = std::move(name)})).get();
        this->index.insert(project);
        return project;
    }

    //! Adds source file identified by `path` to
    //! the Cache. Returns the pointer to the
    //! inserted SourceFile.
    auto add_source_file(std::filesystem::path path) -> SourceFile *
    {
        return Cache::insert_file(std::move(path), this->data.sources);
    }

    //! Finds the header with the exact `path` and
    //! returns it or nullptr if not found.
    [[nodiscard]] auto exact_header_file(const std::filesystem::path &path) -> HeaderFile *
    {
        return this->index.exact_header_file(path);
    }

    //! Finds the source with the exact `path` and
    //! returns it or nullptr if not found.
    [[nodiscard]] auto exact_source_file(const std::filesystem::path &path) -> SourceFile *
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
    [[nodiscard]] auto header_file(const std::filesystem::path &path) -> HeaderFile *
    {
        return this->index.header_file(path);
    }

    //! Find the project with `name`. If no such
    //! project is found throws an out-of-range
    //! exception.
    [[nodiscard]] auto project(const std::string &name) -> Project *
    {
        return this->index.project(name);
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
    [[nodiscard]] auto source_file(const std::filesystem::path &path) -> SourceFile *
    {
        return this->index.source_file(path);
    }

    //! Deleted copy assignment.
    auto operator=(const Cache &other) -> Cache & = delete;

    //! Move assignment.
    auto operator=(Cache &&other) noexcept -> Cache & = default;

private:
    std::filesystem::path filePath;
    CacheData data;
    CacheIndex index;
};
}