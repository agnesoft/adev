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
//! The NodeData is a struct that holds graph
//! node data. It is a POD (plain old data) type that
//! facilitates transfer and storage of values associated
//! with a graph node at a given index.
struct NodeData
{
    //! Represents index pointing to the first outgoing
    //! edge.
    acore::size_type from = acore::INVALID_INDEX;

    //! Represents the number of outgoing edges.
    acore::size_type fromCount = acore::INVALID_INDEX;

    //! Represents index pointing to the first incoming
    //! edge.
    acore::size_type to = acore::INVALID_INDEX;

    //! Represents the number of incoming edges.
    acore::size_type toCount = acore::INVALID_INDEX;
};

//! The EdgeData is a struct that holds graph
//! edge data. It is a POD (plain old data) type that
//! facilitates transfer and storage of values associated
//! with a graph edge at a given index.
struct EdgeData
{
    //! Represents index pointing to the node where this
    //! edge originates.
    acore::size_type from = acore::INVALID_INDEX;

    //! Represents the index of a next edge originating in
    //! the same node.
    acore::size_type nextFrom = acore::INVALID_INDEX;

    //! Represents index pointing to the node where this
    //! edge leads.
    acore::size_type to = acore::INVALID_INDEX;

    //! Represents the index of a next edge pointing in
    //! the same node.
    acore::size_type nextTo = acore::INVALID_INDEX;
};

//! \relates agraph::NodeData
//! Serializes a \a node into the \a stream and
//! returns the \a stream.
template<typename Buffer>
constexpr auto operator<<(acore::DataStreamBase<Buffer> &stream, NodeData node) -> acore::DataStreamBase<Buffer> &
{
    return stream << node.from << node.fromCount << node.to << node.toCount;
}

//! \relates agraph::NodeData
//! Deserializes a \a node from the \a stream and
//! returns the \a stream.
template<typename Buffer>
constexpr auto operator>>(acore::DataStreamBase<Buffer> &stream, NodeData &node) -> acore::DataStreamBase<Buffer> &
{
    return stream >> node.from >> node.fromCount >> node.to >> node.toCount;
}

//! \relates agraph::EdgeData
//! Serializes an \a edge into the \a stream and
//! returns the \a stream.
template<typename Buffer>
constexpr auto operator<<(acore::DataStreamBase<Buffer> &stream, EdgeData edge) -> acore::DataStreamBase<Buffer> &
{
    return stream << edge.from << edge.nextFrom << edge.to << edge.nextTo;
}

//! \relates agraph::EdgeData
//! Deserializes an \a edge from the \a stream and
//! returns the \a stream.
template<typename Buffer>
constexpr auto operator>>(acore::DataStreamBase<Buffer> &stream, EdgeData &edge) -> acore::DataStreamBase<Buffer> &
{
    return stream >> edge.from >> edge.nextFrom >> edge.to >> edge.nextTo;
}

//! Converts the edge \a index into a real index used
//! typically for accessing data stored in arrays.
//!
//! \note The \a index must be less than
//! #acore::INVALID_INDEX.
[[nodiscard]] constexpr auto edgeToIndex(acore::size_type index) noexcept -> acore::size_type
{
    return -(index + 2);
}

//! Converts the real \a index into an edge index that
//! represents an element on a graph.
//!
//! \note The \a index must be greater than
//! #acore::INVALID_INDEX.
[[nodiscard]] constexpr auto indexToEdge(acore::size_type index) noexcept -> acore::size_type
{
    return edgeToIndex(index);
}

//! Returns \c true if the \a index is less than
//! #acore::INVALID_INDEX representing an edge
//! on a graph.
[[nodiscard]] constexpr auto isEdge(acore::size_type index) noexcept -> bool
{
    return index < acore::INVALID_INDEX;
}

//! Returns \c true if the \a index is greater than
//! #acore::INVALID_INDEX representing a node
//! on a graph.
[[nodiscard]] constexpr auto isNode(acore::size_type index) noexcept -> bool
{
    return index > acore::INVALID_INDEX;
}

//! \relates agraph::NodeData
//! Returns \c true if all the member variables are
//! the same in both \a left and \a right.
[[nodiscard]] constexpr auto operator==(NodeData left, NodeData right) noexcept -> bool
{
    return left.from == right.from && left.fromCount == right.fromCount && left.to == right.to && left.toCount == right.toCount;
}

//! \relates agraph::NodeData
//! Returns \c true if any member variable \a left
//! is not the same as in the \a right.
[[nodiscard]] constexpr auto operator!=(NodeData left, NodeData right) noexcept -> bool
{
    return !(left == right);
}

//! \relates agraph::EdgeData
//! Returns \c true if all the member variables are
//! the same in both \a left and \a right.
[[nodiscard]] constexpr auto operator==(EdgeData left, EdgeData right) noexcept -> bool
{
    return left.from == right.from && left.nextFrom == right.nextFrom && left.to == right.to && left.nextTo == right.nextTo;
}

//! \relates agraph::EdgeData
//! Returns \c true if any member variable \a left
//! is not the same as in the \a right.
[[nodiscard]] constexpr auto operator!=(EdgeData left, EdgeData right) noexcept -> bool
{
    return !(left == right);
}
}

#endif
