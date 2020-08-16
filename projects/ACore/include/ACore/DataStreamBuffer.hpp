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

#ifndef ACORE_DATASTREAMBUFFER_HPP
#define ACORE_DATASTREAMBUFFER_HPP

#include "Numeric.hpp"

#include <cstring>
#include <utility>
#include <vector>

namespace acore
{
//! \cond IMPLEMENTAION_DETAIL
class DataStreamBuffer
{
public:
    DataStreamBuffer() = default;

    explicit DataStreamBuffer(std::vector<char> data) noexcept :
        mData{std::move(data)}
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
        std::memcpy(buffer, &mData[offset], static_cast<std::size_t>(count));
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
        std::memcpy(&mData[offset], buffer, static_cast<std::size_t>(count));
    }

private:
    std::vector<char> mData;
};
//! \endcond
}

#endif
