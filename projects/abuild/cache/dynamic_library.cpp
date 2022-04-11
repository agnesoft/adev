#ifndef __clang__
export module abuild.cache:dynamic_library;
export import :linked_file;
#endif

namespace abuild
{
export struct DynamicLibrary : LinkedFile
{
};
}
