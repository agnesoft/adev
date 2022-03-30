#ifndef __clang__
export module abuild.cache : dynamic_library;
export import : binary_file;
#endif

namespace abuild
{
export struct DynamicLibrary : BinaryFile
{
};
}
