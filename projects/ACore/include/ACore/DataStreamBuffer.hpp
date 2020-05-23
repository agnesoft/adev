#ifndef ACORE_DATASTREAMBUFFER_HPP
#define ACORE_DATASTREAMBUFFER_HPP

#include "Numeric.hpp"

#include <cstring>
#include <vector>

namespace acore
{
//! \cond IMPLEMENTAION_DETAIL
class DataStreamBuffer
{
public:
    DataStreamBuffer() = default;

    explicit DataStreamBuffer(std::vector<char> data) :
        mData(std::move(data))
    {
    }

    [[nodiscard]] auto data() const noexcept -> const std::vector<char> &
    {
        return mData;
    }

    [[nodiscard]] auto data() noexcept -> std::vector<char> &
    {
        return mData;
    }

    auto read(size_type offset, char *buffer, size_type count) -> void
    {
        memcpy(buffer, &mData[offset], static_cast<size_t>(count));
    }

    auto resize(size_type size) -> void
    {
        mData.reserve(nextPowerOfTwo(size));
        mData.resize(size);
    }

    [[nodiscard]] auto size() const noexcept -> size_type
    {
        return mData.size();
    }

    auto write(size_type offset, const char *buffer, size_type count) -> void
    {
        memcpy(&mData[offset], buffer, static_cast<size_t>(count));
    }

private:
    std::vector<char> mData;
};
//! \endcond
}

#endif
