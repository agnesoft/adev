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

#ifndef AGRAPH_ELEMENT_HPP
#define AGRAPH_ELEMENT_HPP

#include "AGraphModule.hpp"

namespace agraph
{
//! The Element<ElementType, GraphType> is a template
//! base class for the #agraph::Node and #agraph::Edge}
//! classes.
//!
//! It provides common functionality for the elements
//! of a graph such as the index and the underlying
//! graph instance as well as the common operators.
template<typename ElementType, typename GraphType>
class Element
{
public:
    //! Default constructor.
    constexpr Element() noexcept = default;

    //! Returns the \c GraphType instance passed in the
    //! constructor or nullptr if the element was default
    //! constructed.
    [[nodiscard]] constexpr auto graph() const noexcept -> const GraphType *
    {
        return mGraph;
    }

    //! Returns the \c index of the element or
    //! acore::INVALID_INDEX if the element was
    //! default constructed.
    [[nodiscard]] constexpr auto index() const noexcept -> acore::size_type
    {
        return mIndex;
    }

    //! Returns \c true if the element's \c index
    //! is part of the graph or \c false otherwise.
    [[nodiscard]] constexpr auto isValid() const noexcept -> bool
    {
        return mIndex != acore::INVALID_INDEX && mGraph && mGraph->contains(static_cast<const ElementType &>(*this));
    }

protected:
    //! Constructs the Element with \a index and
    //! \a graph.
    constexpr Element(acore::size_type index, const GraphType *graph) noexcept :
        mIndex{index},
        mGraph{graph}
    {
    }

private:
    acore::size_type mIndex = acore::INVALID_INDEX;
    const GraphType *mGraph = nullptr;
};

//! Returns \c true if the \a left has the same
//! \c index and the \c graph values as the \a right.
template<typename ElementType, typename GraphType>
[[nodiscard]] constexpr auto operator==(Element<ElementType, GraphType> left, Element<ElementType, GraphType> right) noexcept -> bool
{
    return left.graph() == right.graph() && left.index() == right.index();
}

//! Returns \c if the \a left has a different \c index
//! or \c graph values than the \a right.
template<typename ElementType, typename GraphType>
[[nodiscard]] constexpr auto operator!=(Element<ElementType, GraphType> left, Element<ElementType, GraphType> right) noexcept -> bool
{
    return !(left == right);
}
}

#endif
