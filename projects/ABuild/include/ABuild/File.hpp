#ifndef ABUILD_FILE_HPP
#define ABUILD_FILE_HPP

#include <filesystem>

namespace abuild
{
//! \class File File.hpp ABuild.hpp
//! \brief The File class provides meta data
//! relevant for building code about
//! physical files.
class File
{
public:
    //! Constructs the File object from path.
    explicit File(std::filesystem::path path);

    // Returns the path the File.
    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &;

private:
    std::filesystem::path mPath;
};
}

#endif
