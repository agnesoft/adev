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
