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

#ifndef AGRAPH_NODE_HPP
#define AGRAPH_NODE_HPP

#include "Common.hpp"
#include "Element.hpp"

namespace agraph
{
template<typename GraphType, typename GraphImpl>
class Edge;

template<typename GraphType, typename GraphImpl>
class Node : public Element<Node<GraphType, GraphImpl>, GraphType>
{
public:
    using iterator = acore::ForwardIterator<Edge<GraphType, GraphImpl>, Edge<GraphType, GraphImpl>, const Node>;

    constexpr Node() = default;

    [[nodiscard]] constexpr auto begin() const -> iterator
    {
        const acore::size_type edge = this->graph()->mData.node(this->index()).from;

        if (edge < acore::INVALID_INDEX)
        {
            return iterator{edge, this};
        }

        return end();
    }

    [[nodiscard]] constexpr auto edgeCount() const -> acore::size_type
    {
        return fromCount() + toCount();
    }

    [[nodiscard]] constexpr auto end() const noexcept -> iterator
    {
        return iterator{acore::INVALID_INDEX, this};
    }

    [[nodiscard]] constexpr auto fromCount() const -> acore::size_type
    {
        return this->graph()->mData.node(this->index()).fromCount;
    }

    [[nodiscard]] constexpr auto rbegin() const -> iterator
    {
        const acore::size_type edge = this->graph()->mData.node(this->index()).to;

        if (edge < acore::INVALID_INDEX)
        {
            return iterator{edgeToIndex(edge), this};
        }

        return rend();
    }

    [[nodiscard]] constexpr auto rend() const -> iterator
    {
        return end();
    }

    [[nodiscard]] constexpr auto toCount() const -> acore::size_type
    {
        return this->graph()->mData.node(this->index()).toCount;
    }

protected:
    constexpr Node(acore::size_type index, const GraphType *graph) noexcept :
        Element<Node<GraphType, GraphImpl>, GraphType>{index, graph}
    {
    }

private:
    friend iterator;
    friend GraphImpl;

    [[nodiscard]] constexpr auto nextIndex(acore::size_type index) const -> acore::size_type
    {
        if (index < acore::INVALID_INDEX)
        {
            return this->graph()->mData.edge(indexToEdge(index)).nextFrom;
        }

        if (acore::INVALID_INDEX < index)
        {
            return edgeToIndex(this->graph()->mData.edge(index).nextTo);
        }

        return acore::INVALID_INDEX;
    }

    [[nodiscard]] constexpr auto referenceAt(acore::size_type index) const -> Edge<GraphType, GraphImpl>
    {
        if (acore::INVALID_INDEX < index)
        {
            return this->graph()->edge(edgeToIndex(index));
        }

        if (index < acore::INVALID_INDEX)
        {
            return this->graph()->edge(index);
        }

        return this->graph()->edge(acore::INVALID_INDEX);
    }
};
}

#endif
