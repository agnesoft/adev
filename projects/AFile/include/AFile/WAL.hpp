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

#ifndef AFILE_WAL_HPP
#define AFILE_WAL_HPP

#include "FileStream.hpp"

namespace afile
{
//! \cond IMPLEMENTAION_DETAIL
class WAL
{
public:
    struct Log
    {
        acore::size_type pos = acore::INVALID_INDEX;
        std::vector<char> data;
    };

    explicit WAL(FileStream *file);
    WAL(const WAL &other) = delete;
    WAL(WAL &&other) = default;
    ~WAL(); //NOLINT(bugprone-exception-escape)

    auto begin() noexcept -> void;
    auto end() -> void;
    auto recordLog() -> void;
    auto recordLog(acore::size_type pos, acore::size_type count) -> void;
    auto reset() -> void;

    auto operator=(const WAL &other) -> WAL & = delete;
    auto operator=(WAL &&other) -> WAL & = default;

private:
    template<typename Buffer>
    friend constexpr auto operator<<(acore::DataStreamBase<Buffer> &stream, const Log &log) -> acore::DataStreamBase<Buffer> &
    {
        stream << log.pos << static_cast<acore::size_type>(log.data.size());
        stream.write(log.data.data(), log.data.size());
        stream.buffer().flush();
        return stream;
    }

    template<typename Buffer>
    friend constexpr auto operator>>(acore::DataStreamBase<Buffer> &stream, Log &log) -> acore::DataStreamBase<Buffer> &
    {
        acore::size_type size = 0;
        stream >> log.pos >> size;
        log.data.resize(size);
        stream.read(log.data.data(), size);
        return stream;
    }

    auto doReset() -> void;
    auto initialize() -> void;
    [[nodiscard]] auto loadWAL() -> std::vector<Log>;
    [[nodiscard]] auto logWithData(acore::size_type pos, acore::size_type count) -> Log;
    auto processLog(const Log &log) -> void;
    auto processWAL(const std::vector<Log> &logs) -> void;
    [[nodiscard]] auto readLog() -> Log;
    auto saveLog(const Log &log) -> void;

    acore::size_type mWALCount = 0;
    acore::size_type mWALLevel = 0;
    FileStream *mFile = nullptr;
    FileStream mWAL;
};
//! \endcond
}

#endif
