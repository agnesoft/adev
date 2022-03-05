#ifndef __clang__
export module abuild.cache : file;
export import astl;
#endif

namespace abuild
{
//! The `File` is a base struct for all files that
//! are being tracked by the `Cache`.
export struct File
{
    //! Path to the file.
    std::filesystem::path path;

    //! Last modified time of the file.
    std::size_t timestamp = 0;

    //! Size of the file.
    std::size_t size = 0;

    //! Indicates whether the file is outdated
    //! since being cached.
    bool outdated = true;
};
}
