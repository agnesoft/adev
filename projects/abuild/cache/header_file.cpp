#ifndef __clang__
export module abuild.cache : header_file;
export import : source_file_base;
#endif

namespace abuild
{
//! The `HeaderFile` is a base class for C++
//! header files that are being tracked by the
//! `Cache`.
export class HeaderFile : public SourceFileBase
{
public:
    using SourceFileBase::SourceFileBase;
};
}
