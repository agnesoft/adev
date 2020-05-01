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
        TranslationUnit,
        //! Header file (extension (.hpp, .hxx, .h)
        Header,
        //! Not a build file
        Other
    };

    //! Constructs a File object from \a path.
    explicit File(std::filesystem::path path);

    //! Returns contents of the underlying file.
    [[nodiscard]] auto content() const -> std::string;

    //! Returns the path of the underlying file.
    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path &;

    //! Returns the #Type of the file at \a path.
    [[nodiscard]] static auto type(const std::filesystem::path &path) -> Type;

private:
    std::filesystem::path mPath;
};
}

#endif
