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

#ifndef ACORE_DATASTREAM_HPP
#define ACORE_DATASTREAM_HPP

#include "DataStreamBase.hpp"
#include "DataStreamBuffer.hpp"

namespace acore
{
//! The DataStream class provides in-memory
//! serialization of binary data.
//!
//! It is an in-memory specialization of DataStreamBase.
//!
//! Example:
//!
//! \snippet DataStreamTest.cpp [Usage]
class DataStream : public DataStreamBase<DataStreamBuffer>
{
public:
    //! Creates an empty stream object.
    DataStream() = default;

    //! Constructs the stream with \a data as
    //! the internal buffer.
    explicit DataStream(std::vector<char> data) :
        DataStreamBase<DataStreamBuffer>(std::move(data))
    {
    }
};
}

#endif
