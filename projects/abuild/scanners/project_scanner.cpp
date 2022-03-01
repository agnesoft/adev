#ifdef _MSC_VER
export module abuild.scanners : project_scanner;
export import abuild.cache;
#endif

namespace abuild
{
export class ProjectScanner
{
public:
    explicit ProjectScanner(Cache &cache) :
        cache{cache}
    {
    }

    auto scan() -> void
    {
        this->scan_directory(this->cache.project_root());
    }

private:
    auto append_project_name(std::string &projectName, const std::string &directoryName) const -> void
    {
        if (projectName.empty())
        {
            projectName = directoryName;
        }
        else
        {
            projectName += this->cache.settings().projectNameSeparator + directoryName;
        }
    }

    [[nodiscard]] auto ensure_project_name(const std::string &projectName) const -> std::string
    {
        if (projectName.empty())
        {
            return this->cache.project_root().filename().string();
        }
        else
        {
            return projectName;
        }
    }

    [[nodiscard]] auto is_ignore_directory(const std::filesystem::path &path) -> bool
    {
        return path.filename().string().front() == '.' || this->cache.settings().ignoreDirectories.contains(path.filename().string());
    }

    [[nodiscard]] auto is_skip_directory(const std::filesystem::path &path) -> bool
    {
        return this->cache.settings().skipDirectories.contains(path.filename().string());
    }

    [[nodiscard]] auto is_squash_directory(const std::filesystem::path &path) -> bool
    {
        return this->cache.settings().squashDirectories.contains(path.filename().string());
    }

    [[nodiscard]] auto is_test_directory(const std::filesystem::path &path) -> bool
    {
        return this->cache.settings().testDirectories.contains(path.filename().string());
    }

    [[nodiscard]] auto path_directories(std::filesystem::path path) -> std::vector<std::filesystem::path>
    {
        std::vector<std::filesystem::path> directories;

        while (path != this->cache.project_root())
        {
            directories.push_back(path);
            path = path.parent_path();
        }

        return directories;
    }

    auto process_file(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        if (this->cache.settings().cppSourceExtensions.contains(path.extension().string()))
        {
            this->cache.add_source_file(path, projectName);

            if (this->cache.project(projectName)->type != Project::Type::Executable
                && this->cache.settings().executableFilenames.contains(path.stem().string()))
            {
                this->cache.project(projectName)->type = Project::Type::Executable;
            }
        }
        else if (cache.settings().cppHeaderExtensions.contains(path.extension().string()))
        {
            this->cache.add_header_file(path, projectName);
        }
    }

    [[nodiscard]] auto project_name_from_directories(const std::vector<std::filesystem::path> &directories) -> std::string
    {
        std::string projectName;

        for (auto it = directories.crbegin(); it != directories.crend() && !this->is_squash_directory(*it); ++it)
        {
            if (!this->is_skip_directory(*it))
            {
                this->append_project_name(projectName, it->filename().string());
            }
        }

        return this->ensure_project_name(projectName);
    }

    [[nodiscard]] auto project_name_from_path(const std::filesystem::path &path) -> std::string
    {
        return this->project_name_from_directories(this->path_directories(path));
    }

    auto scan_directory(const std::filesystem::path &path) -> void
    {
        const std::string projectName = this->project_name_from_path(path);

        for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(path))
        {
            this->scan_directory_entry(entry, projectName);
        }

        Project *project = cache.project(projectName);

        if (this->is_test_directory(path) && project)
        {
            project->type = Project::Type::Executable;
        }
    }

    auto scan_directory_entry(const std::filesystem::directory_entry &entry, const std::string &projectName) -> void
    {
        const std::filesystem::path entryPath = entry.path();

        if (std::filesystem::is_regular_file(entryPath))
        {
            this->process_file(entryPath, projectName);
        }
        else if (std::filesystem::is_directory(entryPath) && !this->is_ignore_directory(entryPath))
        {
            this->scan_directory(entryPath);
        }
    }

    Cache &cache;
};
}