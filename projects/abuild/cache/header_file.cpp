#ifndef __clang__
export module abuild.cache : header_file;
export import : file;
export import abuild.cpptokenizer;
#endif

namespace abuild
{
//! The `HeaderFile` is a base struct for header
//! files that are being tracked by the `Cache`.
export struct HeaderFile : File
{
    //! Relevant code tokens extracted during code
    //! analysis.
    std::vector<Token> tokens;
};
}
