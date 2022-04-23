#ifndef __clang__
export module abuild.cache:linked_file;
export import :file;
export import :project;
#endif

namespace abuild
{
export struct TranslationUnit;

//! The `LinkedFile` is common base class for
//! linked binary artifacts such as libraries and
//! executables.
export struct LinkedFile : File
{
    //! The underlying project.
    Project *project = nullptr;

    //! List of `TranslationUnit`s that are linked
    //! together into this linked file.
    std::vector<TranslationUnit *> translationUnits;
};
}
