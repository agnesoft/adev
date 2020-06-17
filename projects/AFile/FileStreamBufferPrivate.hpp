#pragma once

#include "AFileModule.hpp"

#include <filesystem>
#include <fstream>
#include <string>

namespace afile
{
class FileStreamBufferPrivate
{
public:
    enum class OpenMode : size_t
    {
        Binary = std::ios_base::binary,
        In = std::ios_base::in,
        Out = std::ios_base::out,
        AtEnd = std::ios_base::ate,
        Truncate = std::ios_base::trunc
    };

    explicit FileStreamBufferPrivate(const char *filename);
    FileStreamBufferPrivate(const FileStreamBufferPrivate &other) = delete;
    FileStreamBufferPrivate(FileStreamBufferPrivate &&other) noexcept = delete;
    ~FileStreamBufferPrivate() noexcept;

    [[nodiscard]] const char *filename() const noexcept;
    void flush();
    [[nodiscard]] bool isOpen() const;
    void read(acore::size_type index, char *buffer, acore::size_type count);
    constexpr void resize(acore::size_type size)
    {
        mSize = size;
    }
    [[nodiscard]] constexpr acore::size_type size() const noexcept
    {
        return mSize;
    }
    void write(acore::size_type index, const char *buffer, acore::size_type count);

    FileStreamBufferPrivate &operator=(const FileStreamBufferPrivate &other) = delete;
    FileStreamBufferPrivate &operator=(FileStreamBufferPrivate &&other) noexcept = delete;

private:
    [[nodiscard]] std::ios_base::openmode openMode(const std::string &filename) const;

    std::string mName;
    std::fstream mFile;
    acore::size_type mSize = 0;
};

[[nodiscard]] FileStreamBufferPrivate::OpenMode operator|(FileStreamBufferPrivate::OpenMode left, FileStreamBufferPrivate::OpenMode right) noexcept;
}
