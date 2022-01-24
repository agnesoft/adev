#ifndef __clang__
export module abuild.cache : source_file_base;
export import : file;
export import abuild.cpptokenizer;
#endif

namespace abuild
{
//! The `SourceFileBase` is a base class for C++
//! source files that are being tracked by the
//! `Cache`.
export class SourceFileBase : public File
{
public:
    //! Constructs the `SourceFileBase` with
    //! `path` and `tokens` and sets the outdated
    //! flag to `outdated`.
    SourceFileBase(std::filesystem::path path, std::vector<Token> tokens, bool outdated) noexcept :
        File{std::move(path), outdated},
        cppTokens{std::move(tokens)}
    {
    }

    //! Returns the tokens passed in during
    //! construction.
    [[nodiscard]] auto tokens() const noexcept -> const std::vector<Token> &
    {
        return this->cppTokens;
    }

private:
    std::vector<Token> cppTokens;
};
}
