#ifndef __clang__
export module abuild.cache:static_library;
export import :linked_file;
#endif

namespace abuild
{
export struct StaticLibrary : LinkedFile
{
};
}
