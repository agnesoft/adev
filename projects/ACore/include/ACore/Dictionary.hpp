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

#ifndef ACORE_DICTIONARY_HPP
#define ACORE_DICTIONARY_HPP

#include "DictionaryBase.hpp"
#include "DictionaryData.hpp"

namespace acore
{
//! The Dictionary is the in-memory reference counted
//! size_type - \c T dictionary.
//!
//! It is a specialization of the DictionaryBase.
//!
//! Example:
//! \snippet DictionaryTest.cpp [Usage]
//!
//! Note that when inserting or removing the same value
//! multiple times the reference counter will change
//! and only when it reaches \c 0 the value gets
//! actually removed from the dictionary.
//!
//! \snippet DictionaryTest.cpp [ReferenceCount]
class Dictionary : public DictionaryBase<DictionaryData>
{
public:
    //! Default constructs and empty dictionary
    Dictionary() = default;

    //! Constructs the dictionary and inserts the
    //! \a values. The same rules apply as with
    //! regular insert() - duplicates and trivial
    //! values are observed as usual. The indexes
    //! assigned start from 0.
    template<typename... Args>
    explicit Dictionary(Args... values)
    {
        insertValues(values...);
    }

private:
    template<typename T, typename... Args>
    auto insertValues(const T &value, Args... values) -> void
    {
        this->insert(value);

        if constexpr (sizeof...(values) > 0)
        {
            insertValues(values...);
        }
    }
};
}

#endif
