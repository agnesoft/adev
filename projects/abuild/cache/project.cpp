#ifndef __clang__
export module abuild.cache : project;
export import astl;
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
    //! Output type of the project.
    enum class Type
    {
        //! Static library producing *.lib or *.a.
        StaticLibrary,

        //! Dynamic library producing *.dll or *.so.
        DynamicLibrary,

        //! Executable producing *.exe (on Windows).
        Executable
    };

    //! Name of the project.
    std::string name;

    //! List of headers that are part of the
    //! project.
    std::vector<HeaderFile *> headers;

    //! List of sources that are part of the
    //! project.
    std::vector<SourceFile *> sources;

    //! Output type of the project.
    Type type = Type::StaticLibrary;
};
}
