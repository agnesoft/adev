#ifndef ABUILD_FILE_HPP
#define ABUILD_FILE_HPP

#include <filesystem>

namespace abuild
{
//! \class File File.hpp ABuild.hpp
//! \brief The File class provides build
//! meta data about physical files.
class File
{
public:
    //! Build type of file
    enum class Type
    {
        //! Source file
        Source,
        //! Header file
        Header,
        //! Not a build file
        Other
    };

    //! Constructs the File object from \a path.
    explicit File(std::filesystem::path path);

    //! Returns the path of the File.
    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &;

    //! Returns the #Type of the underlying file.
    [[nodiscard]] auto type() const noexcept -> Type;

private:
    [[nodiscard]] static auto detectType(const std::filesystem::path &extension) noexcept -> Type;

    std::filesystem::path mPath;
    Type mType = Type::Other;
};
}

#endif
