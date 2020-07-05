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

#ifndef AFILE_AFILEPERSISTENTDICTIONARY_HPP
#define AFILE_AFILEPERSISTENTDICTIONARY_HPP

#include "AFileModule.hpp"
#include "File.hpp"
#include "PersistentDictionaryData.hpp"

namespace afile
{
//! The PersistentDictionary is a persistent
//! reference counted id-value dictionary. It is a
//! specialization of the acore::DictionaryBase
//! using file based data class as its internal
//! storage. As such it provides in-file persistent
//! dictionary with virtually unlimited storage
//! capacity at the cost of disk I/O operations.
//!
//! See the documentation of acore::DictionaryBase
//! for further documentation on common operations
//! and acore::Dictionary for example of usage.
class PersistentDictionary : public acore::DictionaryBase<PersistentDictionaryData>
{
public:
    //! Constructs a new dictionary in the \a file.
    //! The map is assigned a new index().
    explicit PersistentDictionary(File *file);

    //! Constructs a dictionary with existing data
    //! in the \a file at the \a index.
    PersistentDictionary(File *file, acore::size_type index);

    //! Deleted copy constructor.
    PersistentDictionary(const PersistentDictionary &other) = delete;

    //! Move constructor.
    PersistentDictionary(PersistentDictionary &&other) = default;

    //! Destructor.
    ~PersistentDictionary() noexcept = default;

    //! Returns the file this dictionary operates on.
    [[nodiscard]] File *file() const noexcept;

    //! Returns the index of the data of this dictionary
    //! in the file().
    [[nodiscard]] acore::size_type fileIndex() const noexcept;

    //! Converts the dictionary to the memory based
    //! acore::Dictionary.
    [[nodiscard]] acore::Dictionary toDictionary() const;

    //! Deleted copy assignment operator.
    PersistentDictionary &operator=(const PersistentDictionary &other) = delete;

    //! Move assignment operator.
    PersistentDictionary &operator=(PersistentDictionary &&other) noexcept = default;
};
}

#endif
