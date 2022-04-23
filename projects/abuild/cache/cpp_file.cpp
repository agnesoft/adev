#ifndef __clang__
export module abuild.cache:cpp_file;
export import :file;
export import abuild.cpptokenizer;
#endif

namespace abuild
{
export struct Project;

//! The `CppFile` is common base class for C++
//! source files: headers (.hpp) and sources
//! (.cpp).
export struct CppFile : File
{
    //! Project this source file belongs to.
    Project *project = nullptr;

    //! Relevant code tokens extracted during code
    //! analysis.
    std::vector<Token> tokens;
};
}
