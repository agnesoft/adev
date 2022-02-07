#ifndef __clang__
module abuild.cache : cache_index;
import : header_file;
import : project;
import : source_file;
#endif

namespace abuild
{
class CacheIndex
{
    [[nodiscard]] static auto find_file(const std::filesystem::path &path, const auto &index)
    {
        using T = typename std::remove_cvref_t<decltype(index)>::mapped_type;
        const auto range = index.equal_range(path.filename().string());

        for (auto it = range.first; it != range.second; ++it)
        {
            if (CacheIndex::is_same(it->second->path, path))
            {
                return it->second;
            }
        }

        return T{nullptr};
    }

public:
    auto insert(HeaderFile *file) -> void
    {
        this->headers.insert({file->path.filename().string(), file});
    }

    auto insert(Project *project) -> void
    {
        this->projects.insert({project->name, project});
    }

    auto insert(SourceFile *file) -> void
    {
        this->sources.insert({file->path.filename().string(), file});
    }

    [[nodiscard]] auto header_file(const std::filesystem::path &path) -> HeaderFile *
    {
        return CacheIndex::find_file(path, this->headers);
    }

    [[nodiscard]] auto project(const std::string &name) -> Project *
    {
        return this->projects.at(name);
    }

    [[nodiscard]] auto source_file(const std::filesystem::path &path) -> SourceFile *
    {
        return CacheIndex::find_file(path, this->sources);
    }

private:
    [[nodiscard]] static auto is_same(std::filesystem::path left, std::filesystem::path right) -> bool
    {
        while (right.has_parent_path())
        {
            left = left.parent_path();
            right = right.parent_path();

            if (left.filename() != right.filename())
            {
                return false;
            }
        }

        return true;
    }

    std::unordered_multimap<std::string, SourceFile *> sources;
    std::unordered_multimap<std::string, HeaderFile *> headers;
    std::unordered_map<std::string, Project *> projects;
};
}
