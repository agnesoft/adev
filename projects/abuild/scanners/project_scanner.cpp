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

    [[nodiscard]] auto is_executable(Project *project) const -> bool
    {
        return project->type != Project::Type::Executable;
    }

    [[nodiscard]] auto is_executable_filename(const std::filesystem::path &path) const -> bool
    {
        return this->cache.settings().executableFilenames.contains(path.stem().string());
    }

    [[nodiscard]] auto is_ignore_directory(const std::filesystem::path &path) const -> bool
    {
        return path.filename().string().front() == '.' || this->cache.settings().ignoreDirectories.contains(path.filename().string());
    }

    [[nodiscard]] auto is_skip_directory(const std::filesystem::path &path) const -> bool
    {
        return this->cache.settings().skipDirectories.contains(path.filename().string());
    }

    [[nodiscard]] auto is_squash_directory(const std::filesystem::path &path) const -> bool
    {
        return this->cache.settings().squashDirectories.contains(path.filename().string());
    }

    [[nodiscard]] auto is_test_directory(const std::filesystem::path &path) const -> bool
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
            this->process_source_file(path, projectName);
        }
        else if (cache.settings().cppHeaderExtensions.contains(path.extension().string()))
        {
            this->process_header_file(path, projectName);
        }
    }

    template<typename T>
    auto process_cpp_file(T *file) -> void
    {
        const std::size_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(file->path).time_since_epoch()).count();
        const std::size_t size = std::filesystem::file_size(file->path);

        if (file->timestamp != timestamp || file->size != size)
        {
            file->timestamp = timestamp;
            file->size = size;
            file->tokens = ::abuild::tokenize(ProjectScanner::read_file(file->path));
        }
        else
        {
            file->outdated = false;
        }
    }

    [[nodiscard]] auto process_header_file(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        HeaderFile *file = this->cache.exact_header_file(path);

        if (file == nullptr)
        {
            file = this->cache.add_header_file(File{path}, projectName);
        }

        this->process_cpp_file(file);
    }

    [[nodiscard]] auto process_source_file(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        SourceFile *file = this->cache.exact_source_file(path);

        if (file == nullptr)
        {
            file = this->cache.add_source_file(File{path}, projectName);
        }

        this->process_cpp_file(file);

        if (!this->is_executable(file->project) && this->is_executable_filename(path))
        {
            file->project->type = Project::Type::Executable;
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

    [[nodiscard]] static auto read_file(const std::filesystem::path &path) -> std::string
    {
        std::ifstream stream{path};
        stream.seekg(0, std::ios::end);
        const std::size_t size = stream.tellg();
        std::string buffer;
        buffer.reserve(size);
        stream.seekg(0, std::ios::beg);
        stream.read(buffer.data(), size);
        return buffer;
    }

    auto scan_directory(const std::filesystem::path &path) -> void
    {
        const std::string projectName = this->project_name_from_path(path);

        for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(path))
        {
            this->scan_directory_entry(entry, projectName);
        }

        Project *project = cache.project(projectName);

        if (project != nullptr && this->is_test_directory(path))
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