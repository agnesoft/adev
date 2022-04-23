#ifndef __clang__
export module abuild.cache:executable;
export import :linked_file;
#endif

namespace abuild
{
//! The `Executable` represents an executable.
export struct Executable : LinkedFile
{
};
}
