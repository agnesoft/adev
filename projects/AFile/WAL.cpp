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

#include <string>
#include <vector>

namespace afile
{
WAL::WAL(FileStream *file) :
    mFile{file},
    mWAL{("." + std::string(mFile->buffer().filename())).c_str()}
{
    if (mWAL.buffer().isOpen())
    {
        initialize();
    }
    else
    {
        throw acore::Exception() << "Could not open Write Ahead Log file '" << mWAL.buffer().filename() << '\'';
    }
}

WAL::~WAL() //NOLINT(bugprone-exception-escape)
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

void WAL::recordLog()
{
    saveLog(Log{-mFile->buffer().size(), {}});
}

auto WAL::recordLog(acore::size_type pos, acore::size_type count) -> void
{
    if ((pos + count) > mFile->buffer().size())
    {
        acore::size_type actualCount = mFile->buffer().size() - pos;

        if (actualCount > 0)
        {
            saveLog(logWithData(pos, actualCount));
        }

        recordLog();
    }
    else if (count != 0)
    {
        saveLog(logWithData(pos, count));
    }
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

void WAL::initialize()
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
    Log log{pos, {}};
    log.data.resize(static_cast<size_t>(count));
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
        mFile->write(log.data.data(), static_cast<acore::size_type>(log.data.size()));
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

auto WAL::saveLog(const Log &log) -> void
{
    mWAL.seek(mWAL.buffer().size());
    mWAL << log;
    mWAL.reset();
    mWAL << (++mWALCount);
}
}
