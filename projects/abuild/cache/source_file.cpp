#ifndef __clang__
export module abuild.cache:source_file;
export import :file;
export import abuild.cpptokenizer;
#endif

namespace abuild
{
export struct Project;

//! The `SourceFile` is a base struct for source
//! files that are being tracked by the `Cache`.
export struct SourceFile : File
{
    //! Project this source file belongs to.
    Project *project = nullptr;

    //! Relevant code tokens extracted during code
    //! analysis.
    std::vector<Token> tokens;
};
}
