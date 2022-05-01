#ifndef __clang__
export module abuild.cache:project;
export import :file;
#endif

namespace abuild
{
export struct HeaderFile;
export struct SourceFile;

//! The `Project` represents a logical collection
//! of source and header files that is being
//! tracked by the `Cache`.
export struct Project
{
    enum class Type
    {
        Executable,
        DynamicLibrary,
        StaticLibrary
    };

    //! Name of the project.
    std::string name;

    Type type = Type::StaticLibrary;

    File linkedFile;

    //! List of headers that are part of the
    //! project.
    std::vector<HeaderFile *> headers;

    //! List of sources that are part of the
    //! project.
    std::vector<SourceFile *> sources;
};
}
