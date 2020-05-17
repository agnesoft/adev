#ifndef ACORE_ACOREMODULE_HPP
#define ACORE_ACOREMODULE_HPP

//! \defgroup ACoreModule ACore Module

#include <cstdint>

//! \ingroup ACoreModule
//! Contains basic containers and utilities.
namespace acore
{
//! Integer type alias that is guaranteed to be
//! \c signed and exactly \c 64 bit in size.
using size_type = std::int64_t;

//! Invalid index value.
static constexpr size_type INVALID_INDEX = -1;
}

#endif
