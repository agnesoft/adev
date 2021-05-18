#ifdef _MSC_VER
export module abuild : project;
export import<astl.hpp>;
#endif

namespace abuild
{
export class Project
{
public:
    explicit Project(std::string name) :
        mName{std::move(name)}
    {
    }

    [[nodiscard]] auto name() const noexcept -> const std::string &
    {
        return mName;
    }

private:
    std::string mName;
};
}
