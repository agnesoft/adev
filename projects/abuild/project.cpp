#ifdef _MSC_VER
export module abuild : project;
export import<astl.hpp>;
#endif

namespace abuild
{
export class Project
{
public:
    enum class Type
    {
        Executable,
        Library
    };

    explicit Project(std::string name) :
        mName{std::move(name)}
    {
    }

    [[nodiscard]] auto name() const noexcept -> const std::string &
    {
        return mName;
    }

    auto setType(Type projectType) -> void
    {
        mType = projectType;
    }

    [[nodiscard]] auto type() const noexcept -> Type
    {
        return mType;
    }

private:
    std::string mName;
    Type mType = Type::Library;
};
}
