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

#ifndef AGRAPH_PATHSEARCHHANDLER_HPP
#define AGRAPH_PATHSEARCHHANDLER_HPP

#include "AGraphModule.hpp"

namespace agraph
{
struct PathSearchHandler
{
public:
    [[nodiscard]] constexpr auto operator()([[maybe_unused]] acore::size_type index, [[maybe_unused]] acore::size_type distance) const -> acore::size_type
    {
        return 1;
    }
};
}

#endif
