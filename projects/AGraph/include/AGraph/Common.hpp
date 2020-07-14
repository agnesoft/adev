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

#ifndef AGRAPH_COMMON_HPP
#define AGRAPH_COMMON_HPP

#include "AGraphModule.hpp"

namespace agraph
{
struct NodeData
{
    acore::size_type from = acore::INVALID_INDEX;
    acore::size_type fromCount = acore::INVALID_INDEX;
    acore::size_type to = acore::INVALID_INDEX;
    acore::size_type toCount = acore::INVALID_INDEX;
};

struct EdgeData
{
    acore::size_type from = acore::INVALID_INDEX;
    acore::size_type nextFrom = acore::INVALID_INDEX;
    acore::size_type to = acore::INVALID_INDEX;
    acore::size_type nextTo = acore::INVALID_INDEX;
};

template<typename Buffer>
constexpr auto operator<<(acore::DataStreamBase<Buffer> &stream, NodeData node) -> acore::DataStreamBase<Buffer> &
{
    return stream << node.from << node.fromCount << node.to << node.toCount;
}

template<typename Buffer>
constexpr auto operator>>(acore::DataStreamBase<Buffer> &stream, NodeData &node) -> acore::DataStreamBase<Buffer> &
{
    return stream >> node.from >> node.fromCount >> node.to >> node.toCount;
}

template<typename Buffer>
constexpr auto operator<<(acore::DataStreamBase<Buffer> &stream, EdgeData edge) -> acore::DataStreamBase<Buffer> &
{
    return stream << edge.from << edge.nextFrom << edge.to << edge.nextTo;
}

template<typename Buffer>
constexpr auto operator>>(acore::DataStreamBase<Buffer> &stream, EdgeData &edge) -> acore::DataStreamBase<Buffer> &
{
    return stream >> edge.from >> edge.nextFrom >> edge.to >> edge.nextTo;
}

[[nodiscard]] constexpr auto edgeToIndex(acore::size_type index) noexcept -> acore::size_type
{
    return -(index + 2);
}

[[nodiscard]] constexpr auto indexToEdge(acore::size_type index) noexcept -> acore::size_type
{
    return edgeToIndex(index);
}

[[nodiscard]] constexpr auto isEdge(acore::size_type index) noexcept -> bool
{
    return index < acore::INVALID_INDEX;
}

[[nodiscard]] constexpr auto isNode(acore::size_type index) noexcept -> bool
{
    return index > acore::INVALID_INDEX;
}

[[nodiscard]] constexpr auto operator==(NodeData left, NodeData right) noexcept -> bool
{
    return left.from == right.from && left.fromCount == right.fromCount && left.to == right.to && left.toCount == right.toCount;
}

[[nodiscard]] constexpr auto operator!=(NodeData left, NodeData right) noexcept -> bool
{
    return !(left == right);
}

[[nodiscard]] constexpr auto operator==(EdgeData left, EdgeData right) noexcept -> bool
{
    return left.from == right.from && left.nextFrom == right.nextFrom && left.to == right.to && left.nextTo == right.nextTo;
}

[[nodiscard]] constexpr auto operator!=(EdgeData left, EdgeData right) noexcept -> bool
{
    return !(left == right);
}
}

#endif
