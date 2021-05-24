#ifdef _MSC_VER
export module abuild : project;
export import<astl.hpp>;
#endif

namespace abuild
{
export class Source;
export class Header;

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

    auto addHeader(Header *header) -> void
    {
        mHeaders.push_back(header);
    }

    auto addSource(Source *source) -> void
    {
        mSources.push_back(source);
    }

    [[nodiscard]] auto headers() const noexcept -> const std::vector<Header *> &
    {
        return mHeaders;
    }

    [[nodiscard]] auto name() const noexcept -> const std::string &
    {
        return mName;
    }

    auto setType(Type projectType) -> void
    {
        mType = projectType;
    }

    [[nodiscard]] auto sources() const noexcept -> const std::vector<Source *> &
    {
        return mSources;
    }

    [[nodiscard]] auto type() const noexcept -> Type
    {
        return mType;
    }

private:
    std::string mName;
    Type mType = Type::Library;
    std::vector<Source *> mSources;
    std::vector<Header *> mHeaders;
};
}
