#ifndef __clang__
export module abuild.cache:executable;
export import :binary_file;
#endif

namespace abuild
{
export struct Executable : BinaryFile
{
};
}
