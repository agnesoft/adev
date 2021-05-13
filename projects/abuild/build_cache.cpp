#ifdef _MSC_VER
export module abuild : build_cache;
export import : project;
#endif

namespace abuild
{
export class BuildCache
{
public:
    [[nodiscard]] auto addProject(std::string name) -> Project *
    {
        return &mProjects.emplace_back(Project{std::move(name)});
    }

    [[nodiscard]] auto projects() const noexcept -> const std::vector<Project> &
    {
        return mProjects;
    }

private:
    std::vector<Project> mProjects;
};
}