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

#include "FileData.hpp"

#include <algorithm>
#include <string>
#include <vector>

namespace afile
{
FileData::FileData(const char *filename) :
    mFile{filename},
    mWAL{&mFile}
{
    if (!mFile.buffer().isOpen())
    {
        throw acore::Exception() << "Could not open file '." << std::string{mFile.buffer().filename()} << '\'';
    }
}

auto FileData::beginWAL() -> void
{
    mWAL.begin();
}

void FileData::endWAL()
{
    mWAL.end();
}

auto FileData::reset() -> void
{
    mWAL.reset();
    mFile.buffer().resize(static_cast<acore::size_type>(sizeof(acore::size_type)));
    mFile.reset();
    mFile << acore::size_type(0);
}

auto FileData::filename() const noexcept -> const char *
{
    return mFile.buffer().filename();
}

auto FileData::pos() const noexcept -> acore::size_type
{
    return mFile.pos();
}

auto FileData::read(acore::size_type readPos) const -> FileStream &
{
    mFile.seek(readPos);
    return mFile;
}

auto FileData::read(acore::size_type readPos, acore::size_type remainingSize) const -> std::vector<char>
{
    mFile.seek(readPos);
    std::vector<char> data(std::min(MAX_STEP_SIZE, remainingSize));
    mFile.read(data.data(), data.size());
    return data;
}

auto FileData::resize(acore::size_type newSize) -> void
{
    mWAL.recordLog();
    mFile.buffer().resize(newSize);
}

auto FileData::size() const noexcept -> acore::size_type
{
    return mFile.buffer().size();
}

auto FileData::write(acore::size_type pos, const std::vector<char> &data) -> void
{
    mWAL.recordLog(pos, static_cast<acore::size_type>(data.size()));
    mFile.seek(pos);
    mFile.write(data.data(), data.size());
}
}
