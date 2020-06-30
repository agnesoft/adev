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

#ifndef AFILE_FILEDATA_HPP
#define AFILE_FILEDATA_HPP

#include "AFileModule.hpp"
#include "FileStream.hpp"
#include "WAL.hpp"

#include <vector>

namespace afile
{
//! \cond IMPLEMENTAION_DETAIL
class FileData
{
public:
    explicit FileData(const char *filename);

    auto beginWAL() -> void;
    auto endWAL() -> void;
    [[nodiscard]] auto filename() const noexcept -> const char *;

    template<typename T>
    [[nodiscard]] auto load(acore::size_type pos) -> T
    {
        T v{};
        mFile.seek(pos);
        mFile >> v;
        return v;
    }

    [[nodiscard]] auto pos() const noexcept -> acore::size_type;
    [[nodiscard]] auto read(acore::size_type readPos) const -> FileStream &;
    [[nodiscard]] auto read(acore::size_type readPos, acore::size_type size) const -> std::vector<char>;
    auto reset() -> void;
    auto resize(acore::size_type newSize) -> void;

    template<typename T>
    auto save(acore::size_type pos, const T &value)
    {
        write(pos, (acore::DataStream{} << value).buffer().data());
    }

    [[nodiscard]] auto size() const noexcept -> acore::size_type;
    auto write(acore::size_type pos, const std::vector<char> &data) -> void;

private:
    [[nodiscard]] auto validateFile() const -> FileStream *;

    mutable FileStream mFile;
    WAL mWAL;
};
//! \endcond
}

#endif
