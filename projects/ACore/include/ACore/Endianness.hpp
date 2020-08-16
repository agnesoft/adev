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

#ifndef ACORE_ENDIANNESS_HPP
#define ACORE_ENDIANNESS_HPP

#include "ACoreModule.hpp"

namespace acore
{
//! \cond IMPLEMENTAION_DETAIL
struct Endianness
{
    static bool littleEndianness;
};
//! \endcond

//! Declaration for byte swapping functions.
template<typename T>
[[nodiscard]] constexpr auto bswap(T value) -> T;

//! 16-bit specialization of the byte swapping function.
template<>
[[nodiscard]] constexpr auto bswap(std::uint16_t value) -> std::uint16_t
{
    constexpr std::uint16_t BASE = 0U;
    constexpr std::uint16_t ONE_BYTE = 8U;
    constexpr std::uint16_t FIRST_BYTE_MASK = 0x00ffU;
    constexpr std::uint16_t SECOND_BYTE_MASK = 0xff00U;

    return static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(
            BASE
            | static_cast<std::uint16_t>(static_cast<std::uint16_t>(value & FIRST_BYTE_MASK) << ONE_BYTE))
        | static_cast<std::uint16_t>(static_cast<std::uint16_t>(value & SECOND_BYTE_MASK) >> ONE_BYTE));
}

//! 32-bit specialization of the byte swapping function.
template<>
[[nodiscard]] constexpr auto bswap(std::uint32_t value) -> std::uint32_t
{
    constexpr std::uint32_t BASE = 0U;
    constexpr std::uint32_t ONE_BYTE = 8U;
    constexpr std::uint32_t THREE_BYTES = 24U;
    constexpr std::uint32_t FIRST_BYTE_MASK = 0x000000ffU;
    constexpr std::uint32_t SECOND_BYTE_MASK = 0x0000ff00U;
    constexpr std::uint32_t THIRD_BYTE_MASK = 0x00ff0000U;
    constexpr std::uint32_t FOURTH_BYTE_MASK = 0xff000000U;

    return std::uint32_t{BASE
                         | ((value & FIRST_BYTE_MASK) << THREE_BYTES)
                         | ((value & SECOND_BYTE_MASK) << ONE_BYTE)
                         | ((value & THIRD_BYTE_MASK) >> ONE_BYTE)
                         | ((value & FOURTH_BYTE_MASK) >> THREE_BYTES)};
}

//! 64-bit specialization of the byte swapping function.
template<>
[[nodiscard]] constexpr auto bswap(std::uint64_t value) -> std::uint64_t
{
    constexpr std::uint64_t BASE = 0U;
    constexpr std::uint64_t ONE_BYTE = 8U;
    constexpr std::uint64_t THREE_BYTES = 24U;
    constexpr std::uint64_t FIVE_BYTES = 40U;
    constexpr std::uint64_t SEVEN_BYTES = 56U;
    constexpr std::uint64_t FIRST_BYTE_MASK = 0x00000000000000ffU;
    constexpr std::uint64_t SECOND_BYTE_MASK = 0x000000000000ff00U;
    constexpr std::uint64_t THIRD_BYTE_MASK = 0x0000000000ff0000U;
    constexpr std::uint64_t FOURTH_BYTE_MASK = 0x00000000ff000000U;
    constexpr std::uint64_t FIFTH_BYTE_MASK = 0x000000ff00000000U;
    constexpr std::uint64_t SIXTH_BYTE_MASK = 0x0000ff0000000000U;
    constexpr std::uint64_t SEVENTH_BYTE_MASK = 0x00ff000000000000U;
    constexpr std::uint64_t EIGHTH_BYTE_MASK = 0xff00000000000000U;

    return std::uint64_t{BASE
                         | ((value & FIRST_BYTE_MASK) << SEVEN_BYTES)
                         | ((value & SECOND_BYTE_MASK) << FIVE_BYTES)
                         | ((value & THIRD_BYTE_MASK) << THREE_BYTES)
                         | ((value & FOURTH_BYTE_MASK) << ONE_BYTE)
                         | ((value & FIFTH_BYTE_MASK) >> ONE_BYTE)
                         | ((value & SIXTH_BYTE_MASK) >> THREE_BYTES)
                         | ((value & SEVENTH_BYTE_MASK) >> FIVE_BYTES)
                         | ((value & EIGHTH_BYTE_MASK) >> SEVEN_BYTES)};
}

//! Returns \c true if the system's natie byte order
//! is little endian or \c false if it is big endian.
[[nodiscard]] inline auto littleEndian() -> bool
{
    return Endianness::littleEndianness;
}

//! Converts the \a value to little endian byte order.
//! If the system's native byte order is little endian
//! the \a value is simply returned unchanged. If
//! it is big endian the byte swap will be performed
//! and the resulting \c value will be returned.
template<typename T>
[[nodiscard]] constexpr auto toLittleEndian(T value) -> T
{
    if (littleEndian())
    {
        return value;
    }

    return bswap(value);
}

//! Converts the \a value into the little endian byte
//! order. If the system is already little endian
//! the \a value is simply returned unchanged. If
//! the system is big endian the byte swap is performed
//! and the resulting \c value returned.
template<typename T>
[[nodiscard]] constexpr auto fromLittleEndian(T value) -> T
{
    return toLittleEndian(value);
}
}

#endif
