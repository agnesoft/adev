#ifndef __clang__
export module abuild.cache:executable;
export import :linked_file;
#endif

namespace abuild
{
export struct Executable : LinkedFile
{
};
}
