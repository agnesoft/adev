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

#ifndef ACORE_ACOREMODULE_HPP
#define ACORE_ACOREMODULE_HPP

//! \defgroup ACoreModule ACore Module

#include <cstdint>

//! \ingroup ACoreModule
//! The module provides basic containers and utilities.
namespace acore
{
//! Integer type alias that is guaranteed to be
//! \c signed and exactly \c 64 bit in size.
using size_type = std::int64_t;

//! Invalid index value.
static constexpr size_type INVALID_INDEX = -1;
}

#endif
