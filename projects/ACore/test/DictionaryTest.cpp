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

#include <algorithm>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace dictionarytest
{
class DuplicateValue
{
public:
    DuplicateValue() = default;

    explicit DuplicateValue(int i) :
        mValue(i)
    {
    }

    [[nodiscard]] auto operator==(const DuplicateValue &right) const noexcept -> bool
    {
        return mValue == right.mValue;
    }

    [[nodiscard]] auto operator<(const DuplicateValue &right) const noexcept -> bool
    {
        return mValue < right.mValue;
    }

private:
    template<typename Buffer>
    friend auto operator<<(acore::DataStreamBase<Buffer> &stream, const DuplicateValue &value) -> acore::DataStreamBase<Buffer> &
    {
        return stream << value.mValue;
    }

    template<typename Buffer>
    friend auto operator>>(acore::DataStreamBase<Buffer> &stream, DuplicateValue &value) -> acore::DataStreamBase<Buffer> &
    {
        return stream >> value.mValue;
    }

    int mValue = 0;
    std::string mStr;
};
}

namespace std
{
template<>
struct hash<dictionarytest::DuplicateValue>
{
    using argument_type = dictionarytest::DuplicateValue;
    using result_type = std::size_t;

    [[nodiscard]] auto operator()([[maybe_unused]] const argument_type &value) const noexcept -> result_type
    {
        return 1;
    }
};
}

namespace acore
{
[[nodiscard]] auto operator==(const Dictionary &dictionary, const std::vector<Variant> &values) -> bool
{
    return std::equal(dictionary.begin(), dictionary.end(), values.begin());
}
}

namespace dictionarytest
{
TEST_CASE("[acore::Dictionary]")
{
    REQUIRE(std::is_default_constructible_v<acore::Dictionary>);
    REQUIRE(std::is_copy_constructible_v<acore::Dictionary>);
    REQUIRE(std::is_copy_assignable_v<acore::Dictionary>);
    REQUIRE(std::is_nothrow_move_assignable_v<acore::Dictionary>);
    REQUIRE(std::is_nothrow_destructible_v<acore::Dictionary>);
}

TEST_CASE("Dictionary() [acore::Dictionary]")
{
    const acore::Dictionary dictionary;
    REQUIRE(dictionary == std::vector<acore::Variant>{});
}

TEST_CASE("Dictionary(Args... values) [acore::Dictionary]")
{
    const acore::Dictionary dictionary{std::string{"Hello"},
                                       std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                       std::string{"Hello"},
                                       std::string{"World"},
                                       std::vector<char>{'a', 'b'}};
    REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
}

TEST_CASE("Dictionary(const Dictionary &other) [acore::Dictionary]")
{
    const acore::Dictionary dictionary{std::string{"Hello"},
                                       std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                       std::string{"Hello"},
                                       std::string{"World"},
                                       std::vector<char>{'a', 'b'}};
    const acore::Dictionary other{dictionary}; //NOLINT(performance-unnecessary-copy-initialization)
    REQUIRE(other == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
}

TEST_CASE("Dictionary(Dictionary &&other) [acore::Dictionary]")
{
    acore::Dictionary dictionary{std::string{"Hello"},
                                 std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                 std::string{"Hello"},
                                 std::string{"World"},
                                 std::vector<char>{'a', 'b'}};
    const acore::Dictionary other{std::move(dictionary)};
    REQUIRE(other == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
}

TEST_CASE("begin() const noexcept -> const_iterator [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        const acore::Dictionary dictionary;
        REQUIRE(noexcept(dictionary.begin()));
        REQUIRE(dictionary.begin() == dictionary.end());
    }

    SECTION("[data]")
    {
        acore::Dictionary dictionary{std::string{"Hello"},
                                     std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                     std::string{"Hello"},
                                     std::string{"World"},
                                     std::vector<char>{'a', 'b'}};

        REQUIRE(dictionary.begin() != dictionary.end());
        REQUIRE(*dictionary.begin() == acore::Variant{std::string{"Hello"}});
    }
}

TEST_CASE("clear() -> void [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        acore::Dictionary dictionary;
        dictionary.clear();
        REQUIRE(dictionary == std::vector<acore::Variant>{});
    }

    SECTION("[data]")
    {
        acore::Dictionary dictionary{std::vector<int>{5, 7, 9}}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        dictionary.clear();
        REQUIRE(dictionary == std::vector<acore::Variant>{});
    }
}

TEST_CASE("contains(size_type index) const -> bool [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        const acore::Dictionary dictionary;
        REQUIRE_FALSE(dictionary.contains(1));
    }

    SECTION("[data]")
    {
        acore::Dictionary dictionary{std::string{"Hello"}, std::string{"World"}, std::vector<char>{'a', 'b', 'c'}};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).contains(1));
        }

        SECTION("[missing]")
        {
            REQUIRE_FALSE(std::as_const(dictionary).contains(10));
        }

        SECTION("[removed]")
        {
            dictionary.remove(1);
            REQUIRE_FALSE(std::as_const(dictionary).contains(1));
        }
    }
}

TEST_CASE("containsValue(const T &value) const -> bool [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        const acore::Dictionary dictionary;
        REQUIRE_FALSE(dictionary.containsValue(std::string{"Helo"}));
    }

    SECTION("[data]")
    {
        acore::Dictionary dictionary{std::string{"Hello"},
                                     std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                     std::string{"Hello"},
                                     std::string{"World"},
                                     std::vector<char>{'a', 'b'}};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).containsValue(std::string{"Hello"}));
        }

        SECTION("[missing]")
        {
            REQUIRE_FALSE(std::as_const(dictionary).containsValue(std::vector<int>{1, 2, 3, 4}));
        }

        SECTION("[removed]")
        {
            dictionary.remove(0);
            dictionary.remove(0);
            REQUIRE_FALSE(std::as_const(dictionary).containsValue(std::string{"Hello"}));
        }

        SECTION("[trivial]")
        {
            REQUIRE_FALSE(std::as_const(dictionary).containsValue("Hello"));
        }
    }
}

TEST_CASE("count() const noexcept -> size_type [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        const acore::Dictionary dictionary;
        REQUIRE(noexcept(dictionary.count()));
        REQUIRE(dictionary.count() == 0);
    }

    SECTION("[data]")
    {
        const acore::Dictionary dictionary{std::string{"Hello"},
                                           std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                           std::string{"Hello"},
                                           std::string{"World"},
                                           std::vector<char>{'a', 'b'}};

        REQUIRE(dictionary.count() == 4);
    }
}

TEST_CASE("count(size_type index) const -> size_type [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        const acore::Dictionary dictionary;
        REQUIRE(dictionary.count(1) == 0);
    }

    SECTION("[data]")
    {
        acore::Dictionary dictionary{std::string{"Hello"},
                                     std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                     std::string{"Hello"},
                                     std::string{"World"},
                                     std::vector<char>{'a', 'b'}};

        SECTION("[single]")
        {
            REQUIRE(std::as_const(dictionary).count(1) == 1);
        }

        SECTION("[multi]")
        {
            REQUIRE(std::as_const(dictionary).count(0) == 2);
        }

        SECTION("[missing]")
        {
            REQUIRE(std::as_const(dictionary).count(10) == 0);
        }

        SECTION("[removed]")
        {
            dictionary.remove(2);
            REQUIRE(std::as_const(dictionary).count(2) == 0);
        }

        SECTION("[multi removed]")
        {
            dictionary.remove(0);
            REQUIRE(std::as_const(dictionary).count(0) == 1);
        }
    }
}

TEST_CASE("end() const noexcept -> const_iterator [acore::Dictionary]")
{
    const acore::Dictionary dictionary{std::string{"A"}};
    REQUIRE(noexcept(dictionary.end()));
    REQUIRE(dictionary.end() != dictionary.begin());
}

TEST_CASE("index(const T &value) const -> size_type [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        const acore::Dictionary dictionary;
        REQUIRE(dictionary.index(std::string{"Hello"}) == acore::INVALID_INDEX);
    }

    SECTION("[data]")
    {
        acore::Dictionary dictionary{std::string{"Hello"},
                                     std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                     std::string{"Hello"},
                                     std::string{"World"},
                                     std::vector<char>{'a', 'b'}};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).index(std::string{"World"}) == 2);
        }

        SECTION("[missing]")
        {
            REQUIRE(std::as_const(dictionary).index(std::vector<std::string>{"Hello", "World"}) == acore::INVALID_INDEX);
        }

        SECTION("[removed]")
        {
            dictionary.remove(3);
            REQUIRE(std::as_const(dictionary).index(std::vector<char>{'a', 'b'}) == acore::INVALID_INDEX);
        }

        SECTION("[trivial]")
        {
            acore::size_type expected{};
            std::memcpy(&expected, "Hello", 5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(std::as_const(dictionary).index("Hello") == expected);
        }
    }
}

TEST_CASE("insert(const T &value) -> size_type [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        acore::Dictionary dictionary;
        REQUIRE(dictionary.insert(std::string{"Hello"}) == 0);
        REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}});
    }

    SECTION("[data]")
    {
        acore::Dictionary dictionary{std::string{"Hello"},
                                     std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                     std::string{"Hello"},
                                     std::string{"World"},
                                     std::vector<char>{'a', 'b'}};

        SECTION("[existing]")
        {
            REQUIRE(dictionary.insert(std::string{"World"}) == 2);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }

        SECTION("[missing]")
        {
            REQUIRE(dictionary.insert(std::vector<std::string>{"Hi", "There"}) == 4);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}, acore::Variant{std::vector<std::string>{"Hi", "There"}}});
        }

        SECTION("[removed]")
        {
            dictionary.remove(1);
            REQUIRE(dictionary.insert(std::string{"Test"}) == 1);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::string{"Test"}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}, acore::Variant{std::vector<std::string>{"Hi", "There"}}});
        }

        SECTION("[removed multi]")
        {
            dictionary.remove(0);
            REQUIRE(dictionary.insert(std::string{"Hello"}) == 0);
            REQUIRE(dictionary.count(0) == 2);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }

        SECTION("[trivial]")
        {
            acore::size_type expectedIndex{};
            std::memcpy(&expectedIndex, "Hello", 5); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(dictionary.insert("Hello") == expectedIndex);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }
    }
}

TEST_CASE("isEmpty() const noexcept -> bool [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        const acore::Dictionary dictionary;
        REQUIRE(noexcept(dictionary.isEmpty()));
        REQUIRE(dictionary.isEmpty());
    }

    SECTION("[data]")
    {
        const acore::Dictionary dictionary{std::string{"Value"}};
        REQUIRE_FALSE(dictionary.isEmpty());
    }
}

TEST_CASE("remove(size_type index) -> void [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        acore::Dictionary dictionary;
        dictionary.remove(0);
        REQUIRE(dictionary == std::vector<acore::Variant>{});
    }

    SECTION("[data]")
    {
        acore::Dictionary dictionary{std::string{"Hello"},
                                     std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                     std::string{"Hello"},
                                     std::string{"World"},
                                     std::vector<char>{'a', 'b'}};

        SECTION("[single]")
        {
            dictionary.remove(3);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}});
        }

        SECTION("[multi]")
        {
            dictionary.remove(0);
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }

        SECTION("[missing]")
        {
            dictionary.remove(10); //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            REQUIRE(dictionary == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
        }
    }
}

TEST_CASE("storage() noexcept -> Storage * [acore::Dictionary]")
{
    acore::Dictionary dictionary;
    REQUIRE(noexcept(dictionary.storage()));
    REQUIRE(dictionary.storage());
}

TEST_CASE("storage() const noexcept -> const Storage * [acore::Dictionary]")
{
    const acore::Dictionary dictionary;
    REQUIRE(noexcept(dictionary.storage()));
    REQUIRE(dictionary.storage());
}

TEST_CASE("value(size_type index) const -> T [acore::Dictionary]")
{
    SECTION("[empty]")
    {
        const acore::Dictionary dictionary;
        REQUIRE(dictionary.value<std::string>(0) == std::string{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[data]")
    {
        acore::Dictionary dictionary{std::string{"Hello"},
                                     std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                     std::string{"Hello"},
                                     std::string{"World"},
                                     std::vector<char>{'a', 'b'}};

        SECTION("[existing]")
        {
            REQUIRE(std::as_const(dictionary).value<std::vector<int>>(1) == std::vector<int>{1, 2, 3, 4, 5});
        }

        SECTION("[multi]")
        {
            REQUIRE(std::as_const(dictionary).value<std::string>(0) == std::string{"Hello"});
        }

        SECTION("[multi removed]")
        {
            dictionary.remove(0);
            REQUIRE(std::as_const(dictionary).value<std::string>(0) == std::string{"Hello"});
        }

        SECTION("[removed]")
        {
            dictionary.remove(3);
            REQUIRE(std::as_const(dictionary).value<std::vector<char>>(3) == std::vector<char>{}); //NOLINT(readability-container-size-empty)
        }

        SECTION("[trivial]")
        {
            REQUIRE(std::as_const(dictionary).value<char>('A') == 'A');
        }
    }
}

TEST_CASE("operator=(const Dictionary &other) -> Dictionary & [acore::Dictionary]")
{
    const acore::Dictionary dictionary{std::string{"Hello"},
                                       std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                       std::string{"Hello"},
                                       std::string{"World"},
                                       std::vector<char>{'a', 'b'}};
    acore::Dictionary other;
    other = dictionary;
    REQUIRE(other == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
}

TEST_CASE("operator=(Dictionary &&other) -> Dictionary & [acore::Dictionary]")
{
    acore::Dictionary dictionary{std::string{"Hello"},
                                 std::vector<int>{1, 2, 3, 4, 5}, //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                                 std::string{"Hello"},
                                 std::string{"World"},
                                 std::vector<char>{'a', 'b'}};
    acore::Dictionary other;
    other = std::move(dictionary);
    REQUIRE(other == std::vector<acore::Variant>{acore::Variant{std::string{"Hello"}}, acore::Variant{std::vector<int>{1, 2, 3, 4, 5}}, acore::Variant{std::string{"World"}}, acore::Variant{std::vector<char>{'a', 'b'}}});
}

TEST_CASE("acore::Dictionary [examples]")
{
    SECTION("[usage]")
    {
        // clang-format off
        //! [[Usage]]
acore::Dictionary dictionary;

const acore::size_type trivialId = dictionary.insert(4);
const acore::size_type complexId = dictionary.insert(std::string{"Hello!"});
dictionary.insert(std::string{"Hello!"}); //Only increments reference count of complexId

const int i = dictionary.value<int>(trivialId); //10
const auto str = dictionary.value<std::string>(complexId); //"Hello!"
const acore::size_type count = dictionary.count(complexId); //== 2
        //! [[Usage]]
        // clang-format on

        REQUIRE(i == 4);
        REQUIRE(str == "Hello!");
        REQUIRE(!dictionary.contains(trivialId));
        REQUIRE(count == 2);

        // clang-format off
        //! [[ReferenceCount]]
dictionary.remove(complexId); //count(complexId) == 1
dictionary.remove(complexId); //count(complexId) == 0 and actually removes the value
        //! [[ReferenceCount]]
        // clang-format on

        REQUIRE(!dictionary.containsValue(std::string{"Hello!"}));
    }
}
}
