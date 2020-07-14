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

#ifndef AGRAPH_EDGE_HPP
#define AGRAPH_EDGE_HPP

#include "Common.hpp"
#include "Element.hpp"

namespace agraph
{
template<typename GraphType, typename GraphImpl>
class Node;

template<typename GraphType, typename GraphImpl>
class Edge : public Element<Edge<GraphType, GraphImpl>, GraphType>
{
public:
    constexpr Edge() = default;

    [[nodiscard]] constexpr auto from() const -> Node<GraphType, GraphImpl>
    {
        if (this->index() != acore::INVALID_INDEX)
        {
            return this->graph()->node(this->graph()->mData.edge(edgeToIndex(this->index())).from);
        }

        return this->graph()->node(acore::INVALID_INDEX);
    }

    [[nodiscard]] constexpr auto to() const -> Node<GraphType, GraphImpl>
    {
        if (this->index() != acore::INVALID_INDEX)
        {
            return this->graph()->node(this->graph()->mData.edge(edgeToIndex(this->index())).to);
        }

        return this->graph()->node(acore::INVALID_INDEX);
    }

protected:
    constexpr Edge(acore::size_type index, const GraphType *graph) noexcept :
        Element<Edge<GraphType, GraphImpl>, GraphType>{index, graph}
    {
    }

private:
    friend GraphImpl;
};
}

#endif
