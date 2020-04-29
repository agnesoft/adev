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
        //! Source file (extension .cpp, .cxx, .cc or .c)
        Source,
        //! Header file (extension (.hpp, .hxx, .h)
        Header,
        //! Not a build file
        Other
    };

    //! Constructs a File object from \a path.
    explicit File(std::filesystem::path path);

    //! Returns \c true if the underlying file has
    //! changed since the construction of the File object.
    [[nodiscard]] auto isChanged() const -> bool;

    //! Returns the path of the underlying file.
    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &;

    //! Returns the #Type of the underlying file.
    [[nodiscard]] auto type() const noexcept -> Type;

private:
    [[nodiscard]] static auto detectType(const std::filesystem::path &extension) noexcept -> Type;
    [[nodiscard]] static auto lastWriteTime(const std::filesystem::path &path) -> std::filesystem::file_time_type;
    [[nodiscard]] static auto fileContent(const std::filesystem::path &path) -> std::string;

    std::filesystem::path mPath;
    Type mType = Type::Other;
    std::filesystem::file_time_type mLastWrite;
    std::string mContent;
};
}

#endif
