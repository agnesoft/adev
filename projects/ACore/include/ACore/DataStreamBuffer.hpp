#ifndef ACORE_DATASTREAMBUFFER_HPP
#define ACORE_DATASTREAMBUFFER_HPP

#include "ACoreModule.hpp"

#include <vector>

namespace acore
{
//! \cond IMPLEMENTAION_DETAIL
class DataStreamBuffer
{
public:
    DataStreamBuffer() = default;
    explicit DataStreamBuffer(std::vector<char> data);

    [[nodiscard]] auto data() const noexcept -> const std::vector<char> &;
    [[nodiscard]] auto data() noexcept -> std::vector<char> &;
    auto read(size_type offset, char *buffer, size_type count) -> void;
    auto resize(size_type size) -> void;
    [[nodiscard]] auto size() const noexcept -> size_type;
    auto write(size_type offset, const char *buffer, size_type count) -> void;

private:
    std::vector<char> mData;
};
//! \endcond
}

#endif
