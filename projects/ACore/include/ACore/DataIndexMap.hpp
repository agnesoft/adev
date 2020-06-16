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

#ifndef ACORE_DATAINDEXMAP_HPP
#define ACORE_DATAINDEXMAP_HPP

#include "ACoreModule.hpp"
#include "DataIndexMapBase.hpp"
#include "DataIndexMapData.hpp"

#include <utility>
#include <vector>

namespace acore
{
//! The DataIndexMap provides the in-memory
//! \c element-key-value associative container of
//! the acore::size_type values.
//!
//! This class is the specialization of the DataIndexMapBase
//! class using the memory based class as the
//! \c Data argument.
//!
//! Example:
//!
//! \snippet DataIndexMapTest.cpp [Usage]
class DataIndexMap : public DataIndexMapBase<DataIndexMapData>
{
public:
    //! Default constructs an empty map.
    DataIndexMap() = default;

    //! Constructs the map with \a values.
    explicit DataIndexMap(const std::vector<std::pair<size_type, DataIndexMapElement>> &data)
    {
        for (auto val : data)
        {
            this->insert(val.first, val.second.key, val.second.value);
        }
    }
};
}

#endif
