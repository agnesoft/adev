#ifndef __clang__
export module abuild.cache:dynamic_library;
export import :linked_file;
#endif

namespace abuild
{
//! The `DynamicLibrary` represents a dynamic
//! library.
export struct DynamicLibrary : LinkedFile
{
};
}
