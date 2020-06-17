#include "pch.hpp"

#include "FileStreamBufferPrivate.hpp"

namespace afile
{
FileStreamBufferPrivate::FileStreamBufferPrivate(const char *filename) :
    mName(filename),
    mFile(filename, openMode(filename)),
    mSize(static_cast<acore::size_type>(std::filesystem::file_size(filename)))

{
}

FileStreamBufferPrivate::~FileStreamBufferPrivate() noexcept
{
    if (mFile.is_open())
    {
        mFile.close();
        std::filesystem::resize_file(mName, static_cast<size_t>(mSize));
    }
}

const char *FileStreamBufferPrivate::filename() const noexcept
{
    return mName.c_str();
}

void FileStreamBufferPrivate::flush()
{
    mFile.flush();
}

bool FileStreamBufferPrivate::isOpen() const
{
    return mFile.is_open();
}

void FileStreamBufferPrivate::read(acore::size_type index, char *buffer, acore::size_type count)
{
    mFile.seekg(index);
    mFile.read(buffer, count);
}

void FileStreamBufferPrivate::write(acore::size_type index, const char *buffer, acore::size_type count)
{
    mFile.seekp(index);
    mFile.write(buffer, count);
}

std::ios_base::openmode FileStreamBufferPrivate::openMode(const std::string &filename) const
{
    if (std::filesystem::exists(filename))
    {
        return static_cast<std::ios_base::openmode>(OpenMode::Binary | OpenMode::In | OpenMode::Out | OpenMode::AtEnd);
    }

    return static_cast<std::ios_base::openmode>(OpenMode::Binary | OpenMode::In | OpenMode::Out | OpenMode::Truncate);
}

FileStreamBufferPrivate::OpenMode operator|(FileStreamBufferPrivate::OpenMode left, FileStreamBufferPrivate::OpenMode right) noexcept
{
    return static_cast<FileStreamBufferPrivate::OpenMode>(static_cast<std::underlying_type<FileStreamBufferPrivate::OpenMode>::type>(left) | static_cast<std::underlying_type<FileStreamBufferPrivate::OpenMode>::type>(right));
}
}
