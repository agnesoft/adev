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

#ifndef AFILE_PERSISTENTDATAINDEXMAP_HPP
#define AFILE_PERSISTENTDATAINDEXMAP_HPP

#include "PersistentDataIndexMapData.hpp"

namespace afile
{
//! The PersistentDataIndexMap provides the file based
//! \c element-key-value associative container of
//! the acore::acore::size_type values.
//!
//! This class is the specialization of the acore::DataIndexMapBase
//! class using the file based data class as the
//! \c Data argument.
class PersistentDataIndexMap : public acore::DataIndexMapBase<PersistentDataIndexMapData>
{
public:
    //! Constructs a new map in the \a file. The map
    //! is assigned a new index().
    explicit PersistentDataIndexMap(File *file) :
        acore::DataIndexMapBase<PersistentDataIndexMapData>{file}
    {
    }

    //! Constructs a map with existing data in the
    //! \a file at the \a index.
    PersistentDataIndexMap(File *file, acore::size_type index) :
        acore::DataIndexMapBase<PersistentDataIndexMapData>{file, index}
    {
    }

    //! Deleted copy constructor.
    PersistentDataIndexMap(const PersistentDataIndexMap &other) = delete;

    //! Move constructor.
    PersistentDataIndexMap(PersistentDataIndexMap &&other) noexcept = default;

    //! Destructor.
    ~PersistentDataIndexMap() = default;

    //! Returns the file this map operates on.
    [[nodiscard]] constexpr auto file() const noexcept -> File *
    {
        return storage()->file();
    }

    //! Returns the index of the data of this map
    //! in the file().
    [[nodiscard]] constexpr auto fileIndex() const noexcept -> acore::size_type
    {
        return storage()->fileIndex();
    }

    //! Converts the map to the memory based acore::DataIndexMap.
    [[nodiscard]] auto toDataIndexMap() const -> acore::DataIndexMap;

    //! Deleted copy assignment operator.
    auto operator=(const PersistentDataIndexMap &other) -> PersistentDataIndexMap & = delete;

    //! Move assignment operator.
    auto operator=(PersistentDataIndexMap &&other) noexcept -> PersistentDataIndexMap & = default;
};
}

#endif
