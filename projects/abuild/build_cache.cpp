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
        Source *source = mSources.emplace_back(std::make_unique<Source>(path, project(projectName))).get();
        mSourceIndex.insert({path.filename().string(), source});
    }

    auto addHeader(const std::filesystem::path &path, const std::string &projectName) -> void
    {
        Header *header = mHeaders.emplace_back(std::make_unique<Header>(path, project(projectName))).get();
        mHeaderIndex.insert({path.filename().string(), header});
    }

    [[nodiscard]] auto headers() const noexcept -> const std::vector<std::unique_ptr<Header>> &
    {
        return mHeaders;
    }

    [[nodiscard]] auto projects() const noexcept -> const std::vector<std::unique_ptr<Project>> &
    {
        return mProjects;
    }

    [[nodiscard]] auto sources() const noexcept -> const std::vector<std::unique_ptr<Source>> &
    {
        return mSources;
    }

private:
    [[nodiscard]] auto project(const std::string &name) -> Project *
    {
        std::unordered_map<std::string, Project *>::iterator it = mProjectIndex.find(name);

        if (it == mProjectIndex.end())
        {
            Project *p = mProjects.emplace_back(std::make_unique<Project>(name)).get();
            it = mProjectIndex.insert({name, p}).first;
        }

        return it->second;
    }

    std::vector<std::unique_ptr<Project>> mProjects;
    std::vector<std::unique_ptr<Source>> mSources;
    std::vector<std::unique_ptr<Header>> mHeaders;
    std::unordered_map<std::string, Project *> mProjectIndex;
    std::unordered_multimap<std::string, Source *> mSourceIndex;
    std::unordered_multimap<std::string, Header *> mHeaderIndex;
};
}