#ifndef ABUILD_SOURCE_HPP
#define ABUILD_SOURCE_HPP

#include "File.hpp"

#include <optional>
#include <string>
#include <vector>

namespace abuild
{
//! \class Source Source.hpp ABuild.hpp
//! \brief The Source class provides common
//! base for C++ source files.
//!
//! The class provides the includes() and the
//! ability to detect changes to the underlying
//! file and to update() itself on demand.
class Source : public File
{
public:
    //! Constructs a Source object.
    explicit Source(const std::filesystem::path &path);

    //! Returns all the included files.
    [[nodiscard]] auto includes() const noexcept -> const std::vector<std::string> &;

    //! Reloads the source file if it has
    //! changed. Returns true only if the
    //! file was changed (and thus updated)
    //! or false otherwise.
    [[nodiscard]] auto update() -> bool;

private:
    auto extractInclude(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator;
    auto extractIncludes(const std::string &sourceContent) -> void;
    [[nodiscard]] auto lastWriteTime() const -> std::filesystem::file_time_type;
    auto saveInclude(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator;
    auto scanContent() -> bool;
    static auto skipComment(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator;
    static auto skipInclude(std::string::const_iterator it, std::string::const_iterator end) -> std::optional<std::string::const_iterator>;
    static auto skipLineComment(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator;
    static auto skipMultiLineComment(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator;
    static auto skipStringLiteral(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator;
    static auto skipWhiteSpace(std::string::const_iterator it, std::string::const_iterator end) -> std::string::const_iterator;

    std::size_t mHash = 0;
    std::filesystem::file_time_type mLastWriteTime;
    std::vector<std::string> mIncludes;
};
}

#endif
