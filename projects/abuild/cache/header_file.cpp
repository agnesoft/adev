#ifndef __clang__
export module abuild.cache : header_file;
export import : cpp_file;
#endif

namespace abuild
{
//! The `HeaderFile` is a base struct for header
//! files that are being tracked by the `Cache`.
export struct HeaderFile : CppFile
{
};
}
