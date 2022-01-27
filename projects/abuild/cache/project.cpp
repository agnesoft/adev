#ifndef __clang__
export module abuild.cache : project;
export import astl;
#endif

namespace abuild
{
export class HeaderFile;
export class SourceFile;

//! The `Project` represents a logical collection
//! of source and header files that is being
//! tracked by the `Cache`.
export class Project
{
public:
    //! Constructs the project with `name`.
    explicit Project(std::string name) :
        projectName{std::move(name)}
    {
    }

    //! Adds a pointer to HeaderFile to the
    //! project.
    auto add_header_file(HeaderFile *file) -> void
    {
        this->headers.push_back(file);
    }

    //! Adds a pointer to SourceFile to the
    //! project.
    auto add_source_file(SourceFile *file) -> void
    {
        this->sources.push_back(file);
    }

    //! Returns the header files that are part of
    //! the project.
    [[nodiscard]] auto header_files() const noexcept -> const std::vector<HeaderFile *> &
    {
        return this->headers;
    }

    //! Returns the name of the project.
    [[nodiscard]] auto name() const noexcept -> const std::string &
    {
        return this->projectName;
    }

    //! Returns the source files that are part of
    //! the project.
    [[nodiscard]] auto source_files() const noexcept -> const std::vector<SourceFile *> &
    {
        return this->sources;
    }

private:
    std::string projectName;
    std::vector<HeaderFile *> headers;
    std::vector<SourceFile *> sources;
};
}
