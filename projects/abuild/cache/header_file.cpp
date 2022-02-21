#ifndef __clang__
export module abuild.cache : header_file;
export import : file;
export import abuild.cpptokenizer;
#endif

namespace abuild
{
export class Project;

//! The `HeaderFile` is a base struct for header
//! files that are being tracked by the `Cache`.
export struct HeaderFile : File
{
    //! Project this header file belongs to.
    Project *project = nullptr;

    //! Relevant code tokens extracted during code
    //! analysis.
    std::vector<Token> tokens;
};
}
