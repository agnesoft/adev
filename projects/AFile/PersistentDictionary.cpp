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

#include "pch.hpp"

#include "PersistentDictionary.hpp"

namespace afile
{
PersistentDictionary::PersistentDictionary(File *file) :
    acore::DictionaryBase<PersistentDictionaryData>{file}
{
}

PersistentDictionary::PersistentDictionary(File *file, acore::size_type index) :
    acore::DictionaryBase<PersistentDictionaryData>(file, index)
{
}

File *PersistentDictionary::file() const noexcept
{
    return storage()->file();
}

acore::size_type PersistentDictionary::fileIndex() const noexcept
{
    return storage()->fileIndex();
}

acore::Dictionary PersistentDictionary::toDictionary() const
{
    acore::Dictionary dictionary;

    for (auto it = begin(); it != end(); ++it)
    {
        for (acore::size_type i = 0; i < count(it.index()); i++)
        {
            dictionary.insert(*it);
        }
    }

    return dictionary;
}
}
