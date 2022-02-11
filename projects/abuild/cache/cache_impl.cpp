#ifndef __clang__
export module abuild.cache : cache_impl;
import : cache_data;
import : cache_index;
#endif

namespace abuild
{
export class Cache;
auto read_cache(const std::filesystem::path &path, Cache &cache) -> void;
auto write_cache(const std::filesystem::path &path, const CacheData &data) -> void;

export class Cache
{
    [[nodiscard]] auto insert_file(std::filesystem::path path, auto &files)
    {
        using T = typename std::remove_cvref_t<decltype(files)>::value_type::element_type;
        T *file = files.emplace_back(new T{File{.path = std::move(path)}}).get();
        this->index.insert(file);
        return file;
    }

public:
    explicit Cache(std::filesystem::path path) :
        filePath{std::move(path)}
    {
        if (std::filesystem::exists(this->filePath))
        {
            ::abuild::read_cache(this->filePath, *this);
        }
    }

    auto add_header_file(std::filesystem::path path) -> HeaderFile *
    {
        return Cache::insert_file(std::move(path), this->data.headers);
    }

    auto add_project(std::string name) -> Project *
    {
        Project *project = this->data.projects.emplace_back(new Project{.name = std::move(name)}).get();
        this->index.insert(project);
        return project;
    }

    auto add_source_file(std::filesystem::path path) -> SourceFile *
    {
        return Cache::insert_file(std::move(path), this->data.sources);
    }

    [[nodiscard]] auto header_file(const std::filesystem::path &path) -> HeaderFile *
    {
        return this->index.header_file(path);
    }

    [[nodiscard]] auto project(const std::string &name) -> Project *
    {
        return this->index.project(name);
    }

    [[nodiscard]] auto source_file(const std::filesystem::path &path) -> SourceFile *
    {
        return this->index.source_file(path);
    }

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

private:
    std::filesystem::path filePath;
    CacheData data;
    CacheIndex index;
};
}