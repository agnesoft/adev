#ifndef __clang__
module abuild.cache : cache_index;
import : header_file;
import : project;
import : source_file;
#endif

namespace abuild
{
//! \private
class CacheIndex
{
    [[nodiscard]] static auto find_file(const std::filesystem::path &path, const auto &index) -> auto
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

    [[nodiscard]] static auto find_exact_file(const std::filesystem::path &path, const auto &index) -> auto
    {
        using T = typename std::remove_cvref_t<decltype(index)>::mapped_type;
        const auto it = index.find(path);

        if (it != index.end())
        {
            return it->second;
        }

        return T{nullptr};
    }

public:
    [[nodiscard]] auto exact_header_file(const std::filesystem::path &path) const -> HeaderFile *
    {
        return CacheIndex::find_exact_file(path, this->headers);
    }

    [[nodiscard]] auto exact_source_file(const std::filesystem::path &path) const -> SourceFile *
    {
        return CacheIndex::find_exact_file(path, this->sources);
    }

    auto insert(HeaderFile *file) -> void
    {
        this->headersByName.insert({file->path.filename().string(), file});
        this->headers.insert({file->path, file});
    }

    auto insert(Project *project) -> void
    {
        this->projects.insert({project->name, project});
    }

    auto insert(SourceFile *file) -> void
    {
        this->sourcesByName.insert({file->path.filename().string(), file});
        this->sources.insert({file->path, file});
    }

    [[nodiscard]] auto header_file(const std::filesystem::path &path) const -> HeaderFile *
    {
        return CacheIndex::find_file(path, this->headersByName);
    }

    [[nodiscard]] auto project(const std::string &name) const -> Project *
    {
        auto it = this->projects.find(name);

        if (it != this->projects.end())
        {
            return it->second;
        }

        return nullptr;
    }

    [[nodiscard]] auto source_file(const std::filesystem::path &path) const -> SourceFile *
    {
        return CacheIndex::find_file(path, this->sourcesByName);
    }

private:
    struct PathHash
    {
        [[nodiscard]] auto operator()(std::filesystem::path const &path) const noexcept -> std::size_t
        {
            return std::filesystem::hash_value(path);
        }
    };

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

    std::unordered_multimap<std::string, SourceFile *> sourcesByName;
    std::unordered_multimap<std::string, HeaderFile *> headersByName;
    std::unordered_map<std::filesystem::path, SourceFile *, PathHash> sources;
    std::unordered_map<std::filesystem::path, HeaderFile *, PathHash> headers;
    std::unordered_map<std::string, Project *> projects;
};
}
