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

#include "WAL.hpp"

namespace afile
{
WAL::WAL(FileStream *file) :
    mFile{file},
    mWAL{("." + std::string(file->buffer().filename())).c_str()}
{
    if (mFile->buffer().isOpen() && mWAL.buffer().isOpen())
    {
        process();
    }
    else
    {
        throw acore::Exception() << "Could not open Write Ahead Log file '." << mWAL.buffer().filename() << '\'';
    }
}

WAL::~WAL()
{
    processWAL(loadWAL());
}

auto WAL::begin() noexcept -> void
{
    mWALLevel++;
}

auto WAL::end() -> void
{
    mWALLevel--;
    reset();
}

auto WAL::reset() -> void
{
    if (mWALLevel == 0)
    {
        doReset();
    }
}

auto WAL::doReset() -> void
{
    mWAL.buffer().resize(0);
    mWAL.reset();
    mWALCount = 0;
    mWAL << mWALCount;
    mWAL.buffer().flush();
}

auto WAL::loadWAL() -> std::vector<WAL::Log>
{
    std::vector<Log> logs;
    mWAL.reset();
    mWAL >> mWALCount;
    logs.reserve(mWALCount);

    for (acore::size_type i = 0; i < mWALCount; i++)
    {
        logs.emplace_back(readLog());
    }

    return logs;
}

auto WAL::logWithData(acore::size_type pos, acore::size_type count) -> Log
{
    Log log;
    log.pos = pos;
    log.data.resize(count);
    mFile->seek(pos);
    mFile->read(log.data.data(), count);
    return log;
}

auto WAL::processLog(const Log &log) -> void
{
    if (log.pos < 0)
    {
        mFile->buffer().resize(-log.pos);
    }
    else
    {
        mFile->seek(log.pos);
        mFile->write(log.data.data(), log.data.size());
    }
}

void WAL::process()
{
    if (mWAL.buffer().size() != 0)
    {
        processWAL(loadWAL());
    }
    else
    {
        mWAL << mWALCount;
    }
}

void WAL::processWAL(const std::vector<Log> &logs)
{
    for (auto it = logs.rbegin(); it != logs.rend(); ++it)
    {
        processLog(*it);
    }

    mWAL.reset();
}

auto WAL::readLog() -> Log
{
    Log log;
    mWAL >> log;
    return log;
}

auto WAL::recordLog(const Log &log) -> void
{
    mWAL.seek(mWAL.buffer().size());
    mWAL << log;
    mWAL.reset();
    mWAL << (++mWALCount);
}

auto WAL::recordLog(acore::size_type pos, acore::size_type count) -> void
{
    if ((pos + count) > mFile->buffer().size())
    {
        acore::size_type actualCount = mFile->buffer().size() - pos;

        if (actualCount > 0)
        {
            recordLog(logWithData(pos, actualCount));
        }

        recordLog(Log{-mFile->buffer().size(), {}});
    }
    else if (count != 0)
    {
        recordLog(logWithData(pos, count));
    }
}
}
