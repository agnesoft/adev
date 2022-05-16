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
    //! Type of the project.
    enum class Type
    {
        //! The project is an executable file.
        Executable,

        //! The project is a dynamic library.
        DynamicLibrary,

        //! The project is a static library.
        StaticLibrary
    };

    //! Name of the project.
    std::string name;

    //! Type of the project.
    Type type = Type::StaticLibrary;

    //! The output file of the project.
    File linkedFile;

    //! List of headers that are part of the
    //! project.
    std::vector<HeaderFile *> headers;

    //! List of sources that are part of the
    //! project.
    std::vector<SourceFile *> sources;
};
}
