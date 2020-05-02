#ifndef ABUILD_SOURCE_HPP
#define ABUILD_SOURCE_HPP

#include "File.hpp"

#include <string>
#include <vector>

namespace abuild
{
//! \class Source Source.hpp ABuild.hpp
//! \brief The Source class provides common
//! base for C++ source files.
//!
//! The class provides the ability to detect
//! changes to the underlying file and to
//! update() the meta data.
class Source : public File
{
public:
    //! Constructs a Source object.
    explicit Source(const std::filesystem::path &path);

    //! Returns all included files of this
    //! source file.
    [[nodiscard]] auto includes() const noexcept -> const std::vector<std::string> &;

    //! Reloads the source file if it has
    //! changed. Returns true only if the
    //! file was changed (and thus updated)
    //! or false otherwise.
    [[nodiscard]] auto update() -> bool;

private:
    [[nodiscard]] auto lastWriteTime() const -> std::filesystem::file_time_type;
    auto scanContent() -> bool;

    std::filesystem::file_time_type mLastWriteTime;
    std::size_t mHash = 0;
    std::vector<std::string> mIncludes;
};
}

#endif
