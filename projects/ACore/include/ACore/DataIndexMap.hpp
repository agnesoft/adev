#pragma once

#include "DataIndexMapBase.hpp"
#include "DataIndexMapData.hpp"

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
