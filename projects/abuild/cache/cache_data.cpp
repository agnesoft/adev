#ifndef __clang__
module abuild.cache : cache_data;
import : header_file;
import : project;
import : source_file;
import : settings;
#endif

namespace abuild
{
//! \private
struct CacheData
{
    std::vector<std::unique_ptr<SourceFile>> sources;
    std::vector<std::unique_ptr<HeaderFile>> headers;
    std::vector<std::unique_ptr<Project>> projects;
    Settings settings;
};
}
