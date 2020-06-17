// Copyright 2020 Michael Vlach
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "pch.hpp"

#include "FileStreamBuffer.hpp"
#include "FileStreamBufferPrivate.hpp"

namespace afile
{
FileStreamBuffer::FileStreamBuffer(const char *filename) :
    d(new FileStreamBufferPrivate(filename))
{
}

FileStreamBuffer::FileStreamBuffer(FileStreamBuffer &&other) noexcept
{
    *this = std::move(other);
}

FileStreamBuffer::~FileStreamBuffer() noexcept
{
    delete d;
}

const char *FileStreamBuffer::filename() const noexcept
{
    return d->filename();
}

void FileStreamBuffer::flush()
{
    d->flush();
}

bool FileStreamBuffer::isOpen() const
{
    return d->isOpen();
}

void FileStreamBuffer::read(acore::size_type index, char *buffer, acore::size_type count)
{
    d->read(index, buffer, count);
}

void FileStreamBuffer::resize(acore::size_type size)
{
    d->resize(size);
}

acore::size_type FileStreamBuffer::size() const noexcept
{
    return d->size();
}

void FileStreamBuffer::write(acore::size_type index, const char *buffer, acore::size_type count)
{
    d->write(index, buffer, count);
}

FileStreamBuffer &FileStreamBuffer::operator=(FileStreamBuffer &&other) noexcept
{
    if (this != &other)
    {
        delete d;
        d = other.d;
        other.d = nullptr;
    }

    return *this;
}
}
