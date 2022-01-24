#ifndef __clang__
export module abuild.cache : source_file;
export import : source_file_base;
#endif

namespace abuild
{
//! The `SourceFile` is a base class for C++
//! source files that are being tracked by the
//! `Cache`.
export class SourceFile : public SourceFileBase
{
public:
    using SourceFileBase::SourceFileBase;
};
}
