#ifdef _MSC_VER
export module abuild : build_cache;
export import : project;
export import : file;
#endif

namespace abuild
{
export class BuildCache
{
public:
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

    [[nodiscard]] auto projects() const noexcept -> const std::vector<Project> &
    {
        return mProjects;
    }

private:
    std::vector<Project> mProjects;
    std::unordered_map<std::string, Project *> mProjectIndex;
};
}