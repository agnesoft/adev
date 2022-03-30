#ifndef __clang__
export module abuild.cache : source_file;
export import : cpp_file;
#endif

namespace abuild
{
//! The `SourceFile` is a base struct for source
//! files that are being tracked by the `Cache`.
export struct SourceFile : CppFile
{
};
}
