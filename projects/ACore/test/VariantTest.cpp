#include "pch.hpp"

#include <Catch2/catch.hpp>

namespace varianttest
{
TEST_CASE("[acore::Variant]")
{
    REQUIRE(std::is_default_constructible_v<acore::Variant>);
    REQUIRE(std::is_copy_constructible_v<acore::Variant>);
    REQUIRE(std::is_nothrow_move_constructible_v<acore::Variant>);
    REQUIRE(std::is_copy_assignable_v<acore::Variant>);
    REQUIRE(std::is_nothrow_move_assignable_v<acore::Variant>);
    REQUIRE(std::is_nothrow_destructible_v<acore::Variant>);
}

TEST_CASE("Variant() [acore::Variant]")
{
    const acore::Variant variant;
    REQUIRE(variant.value<const std::vector<char> &>() == std::vector<char>{}); //NOLINT(readability-container-size-empty)
}

TEST_CASE("Variant(const T &value) [acore::Variant]")
{
    const acore::Variant variant{std::vector<int>{1, 2}};
    REQUIRE(variant.value<const std::vector<char> &>().size() == 16);
}

TEST_CASE("isValid() const noexcept -> bool [acore::Variant]")
{
    SECTION("[empty]")
    {
        const acore::Variant variant;
        REQUIRE(noexcept(variant.isValid()));
        REQUIRE_FALSE(variant.isValid());
    }

    SECTION("[data]")
    {
        const acore::Variant variant{"Hello World!"};
        REQUIRE(variant.isValid());
    }
}

TEST_CASE("value() const -> T [acore::Variant]")
{
    SECTION("[empty]")
    {
        const acore::Variant variant;
        REQUIRE(variant.value<std::string>() == std::string{}); //NOLINT(readability-container-size-empty)
    }

    SECTION("[valid type]")
    {
        const acore::Variant variant{std::string{"Hello World!"}};
        REQUIRE(variant.value<std::string>() == "Hello World!");
    }

    SECTION("[raw data]")
    {
        const acore::Variant variant{std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'}};
        REQUIRE(variant.value<std::vector<char>>() == std::vector<char>{'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'});
    }

    SECTION("[Variant]")
    {
        const acore::Variant variant{std::vector{1, -1, 2, -2, 3, -3, 4, -4}};
        REQUIRE(variant.value<acore::Variant>() == variant);
    }

    SECTION("[invalid type]")
    {
        const acore::Variant variant{10};
        REQUIRE(variant.value<std::vector<int>>() == std::vector<int>{}); //NOLINT(readability-container-size-empty)
    }
}

TEST_CASE("operator==(const Variant &left, const Variant &right) noexcept -> bool [acore::Variant]")
{
    SECTION("[empty]")
    {
        const acore::Variant variant;
        const acore::Variant other;
        REQUIRE(noexcept(variant == other));
        REQUIRE(variant == other);
    }

    SECTION("[equal]")
    {
        const acore::Variant variant{std::string{"Hi there, test!"}};
        const acore::Variant other{std::string{"Hi there, test!"}};
        REQUIRE(variant == other);
    }

    SECTION("[different]")
    {
        const acore::Variant variant{std::string{"Hi there, test!"}};
        const acore::Variant other{std::string{"Hello Wordl!"}};
        REQUIRE_FALSE(variant == other);
    }
}

TEST_CASE("operator!=(const Variant &left, const Variant &right) noexcept -> bool [acore::Variant]")
{
    SECTION("[empty]")
    {
        const acore::Variant variant;
        const acore::Variant other;
        REQUIRE(noexcept(variant != other));
        REQUIRE_FALSE(variant != other);
    }

    SECTION("[equal]")
    {
        const acore::Variant variant{std::string{"Hi there, test!"}};
        const acore::Variant other{std::string{"Hi there, test!"}};
        REQUIRE_FALSE(variant != other);
    }

    SECTION("[different]")
    {
        const acore::Variant variant{std::string{"Hi there, test!"}};
        const acore::Variant other{std::string{"Hello Wordl!"}};
        REQUIRE(variant != other);
    }
}

TEST_CASE("operator<<(DataStreamBase<Buffer> &stream, const Variant &value) -> DataStream, operator>>(DataStreamBase<Buffer> &stream, const Variant &value) -> DataStream [acore::Variant]")
{
    SECTION("[empty]")
    {
        const acore::Variant variant;
        acore::DataStream stream;

        stream << variant;
        REQUIRE(stream.buffer().data().size() == static_cast<acore::size_type>(sizeof(acore::size_type)));
        stream.reset();

        acore::Variant other;
        stream >> other;
        REQUIRE(variant == other);
    }

    SECTION("[data]")
    {
        const acore::Variant variant{std::vector<int>{1, 2, 3, 4, -4}};
        acore::DataStream stream;

        stream << variant;
        REQUIRE(stream.buffer().data().size() == static_cast<acore::size_type>(sizeof(acore::size_type) * 2 + sizeof(int) * 5));
        stream.reset();

        acore::Variant other;
        stream >> other;
        REQUIRE(variant == other);
    }
}
}
