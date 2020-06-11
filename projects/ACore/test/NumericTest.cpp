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

#include <catch2/catch.hpp>

#include <cstdint>

namespace numerictest
{
TEST_CASE("nextPowerOfTwo(size_type value) -> size_type [ACore]")
{
    REQUIRE(acore::nextPowerOfTwo(0) == std::int64_t{2});
    REQUIRE(acore::nextPowerOfTwo(1) == std::int64_t{2});
    REQUIRE(acore::nextPowerOfTwo(3) == std::int64_t{4});
    REQUIRE(acore::nextPowerOfTwo(5) == std::int64_t{8});
    REQUIRE(acore::nextPowerOfTwo(9) == std::int64_t{16});
    REQUIRE(acore::nextPowerOfTwo(4294967297) == std::int64_t{8589934592});
}
}
