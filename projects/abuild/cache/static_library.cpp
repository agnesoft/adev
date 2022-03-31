#ifndef __clang__
export module abuild.cache:static_library;
export import :binary_file;
#endif

namespace abuild
{
export struct StaticLibrary : BinaryFile
{
};
}
