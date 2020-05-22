#include "DataStreamBuffer.hpp"
#include "Numeric.hpp"

#include <cstring>

namespace acore
{
DataStreamBuffer::DataStreamBuffer(std::vector<char> data) :
    mData(std::move(data))
{
}

auto acore::DataStreamBuffer::data() noexcept -> std::vector<char> &
{
    return mData;
}

auto DataStreamBuffer::data() const noexcept -> const std::vector<char> &
{
    return mData;
}

auto DataStreamBuffer::read(size_type offset, char *buffer, size_type count) -> void
{
    memcpy(buffer, &mData[offset], static_cast<size_t>(count));
}

auto DataStreamBuffer::resize(size_type size) -> void
{
    mData.reserve(nextPowerOfTwo(size));
    mData.resize(size);
}

auto DataStreamBuffer::size() const noexcept -> size_type
{
    return mData.size();
}

auto DataStreamBuffer::write(size_type offset, const char *buffer, size_type count) -> void
{
    memcpy(&mData[offset], buffer, static_cast<size_t>(count));
}
}
