#include "pch.hpp"

#include "FileStreamBuffer.hpp"

namespace afile
{
FileStreamBuffer::FileStreamBuffer(const char *filename) :
    mName(filename),
    mFile(filename, openMode(filename)),
    mSize(static_cast<acore::size_type>(std::filesystem::file_size(filename)))

{
}

FileStreamBuffer::~FileStreamBuffer() noexcept
{
    if (mFile.is_open())
    {
        mFile.close();
        std::filesystem::resize_file(mName, static_cast<size_t>(mSize));
    }
}

const char *FileStreamBuffer::filename() const noexcept
{
    return mName.c_str();
}

void FileStreamBuffer::flush()
{
    mFile.flush();
}

bool FileStreamBuffer::isOpen() const
{
    return mFile.is_open();
}

void FileStreamBuffer::read(acore::size_type index, char *buffer, acore::size_type count)
{
    mFile.seekg(index);
    mFile.read(buffer, count);
}

void FileStreamBuffer::write(acore::size_type index, const char *buffer, acore::size_type count)
{
    mFile.seekp(index);
    mFile.write(buffer, count);
}

std::ios_base::openmode FileStreamBuffer::openMode(const std::string &filename) const
{
    if (std::filesystem::exists(filename))
    {
        return static_cast<std::ios_base::openmode>(OpenMode::Binary | OpenMode::In | OpenMode::Out | OpenMode::AtEnd);
    }

    return static_cast<std::ios_base::openmode>(OpenMode::Binary | OpenMode::In | OpenMode::Out | OpenMode::Truncate);
}

FileStreamBuffer::OpenMode operator|(FileStreamBuffer::OpenMode left, FileStreamBuffer::OpenMode right) noexcept
{
    return static_cast<FileStreamBuffer::OpenMode>(static_cast<std::underlying_type<FileStreamBuffer::OpenMode>::type>(left) | static_cast<std::underlying_type<FileStreamBuffer::OpenMode>::type>(right));
}
}
