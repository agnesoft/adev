#ifndef __clang__
export module abuild.cache : source_file_base;
export import : file;
import abuild.cpptokenizer;
#endif

namespace abuild
{
//!
export class SourceFileBase : public File
{
public:
    //!
    explicit SourceFileBase(std::filesystem::path path) noexcept :
        File{std::move(path), true}
    {
    }

    //!
    SourceFileBase(std::filesystem::path path, std::vector<Token> tokens) noexcept :
        File{std::move(path), false},
        cppTokens{std::move(tokens)}
    {
    }

    [[nodiscard]] auto tokens() const noexcept -> const std::vector<Token> &
    {
        return this->cppTokens;
    }

private:
    std::vector<Token> cppTokens;
};
}
