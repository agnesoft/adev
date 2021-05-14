#ifdef _MSC_VER
export module abuild : build_cache;
export import : project;
export import : header;
export import : source;
#endif

namespace abuild
{
export class BuildCache
{
public:
    auto addSource(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        Source *source = &mSources.emplace_back(Source{path, project(projectName)});
        mSourceIndex.insert({path.filename().string(), source});
    }

    auto addHeader(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        Header *header = &mHeaders.emplace_back(Header{path, project(projectName)});
        mHeaderIndex.insert({path.filename().string(), header});
    }

    [[nodiscard]] auto headers() const noexcept -> const std::vector<Header> &
    {
        return mHeaders;
    }

    [[nodiscard]] auto projects() const noexcept -> const std::vector<Project> &
    {
        return mProjects;
    }

    [[nodiscard]] auto sources() const noexcept -> const std::vector<Source> &
    {
        return mSources;
    }

private:
    [[nodiscard]] auto project(const std::string &name) -> Project *
    {
        std::unordered_map<std::string, Project *>::iterator it = mProjectIndex.find(name);

        if (it == mProjectIndex.end())
        {
            Project *p = &mProjects.emplace_back(Project{name});
            it = mProjectIndex.insert({name, p}).first;
        }

        return it->second;
    }

    std::vector<Project> mProjects;
    std::vector<Source> mSources;
    std::vector<Header> mHeaders;
    std::unordered_map<std::string, Project *> mProjectIndex;
    std::unordered_multimap<std::string, Source *> mSourceIndex;
    std::unordered_multimap<std::string, Header *> mHeaderIndex;
};
}