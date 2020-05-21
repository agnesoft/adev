#include "pch.hpp"

#include <Catch2/catch.hpp>

#include <sstream>

namespace randomaccessiteratortest
{
class Iterable
{
public:
    using iterator = acore::RandomAccessIterator<int, int &, Iterable>;
    using const_iterator = acore::RandomAccessIterator<const int, const int, const Iterable>;

    [[nodiscard]] auto data() const noexcept -> const std::vector<int> &
    {
        return mData;
    }

    [[nodiscard]] static auto distance(acore::size_type left, acore::size_type right) -> acore::size_type
    {
        return left - right;
    }

    [[nodiscard]] static auto nextIndex(acore::size_type index, acore::size_type steps) -> acore::size_type
    {
        return index + steps;
    }

    [[nodiscard]] static auto previousIndex(acore::size_type index, acore::size_type steps) -> acore::size_type
    {
        return index - steps;
    }

    [[nodiscard]] auto referenceAt(acore::size_type index) -> int &
    {
        return mData[index];
    }

    [[nodiscard]] auto referenceAt(acore::size_type index) const -> int
    {
        return mData[index];
    }

private:
    std::vector<int> mData = {1, 2, 3, 4, 5}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
};
}

namespace Catch
{
template<>
struct StringMaker<randomaccessiteratortest::Iterable::iterator>
{
    [[nodiscard]] static auto convert(const randomaccessiteratortest::Iterable::iterator &it) -> std::string
    {
        std::stringstream os;
        os << '{' << it.index() << ", ";

        if (it.iterable() != nullptr)
        {
            os << it.iterable();
        }
        else
        {
            os << "nullptr";
        }

        os << '}';
        return os.str();
    }
};

template<>
struct StringMaker<randomaccessiteratortest::Iterable::const_iterator>
{
    [[nodiscard]] static auto convert(const randomaccessiteratortest::Iterable::const_iterator &it) -> std::string
    {
        std::stringstream os;
        os << '{' << it.index() << ", ";

        if (it.iterable() != nullptr)
        {
            os << it.iterable();
        }
        else
        {
            os << "nullptr";
        }

        os << '}';
        return os.str();
    }
};
}

namespace randomaccessiteratortest
{
TEMPLATE_TEST_CASE("[acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    REQUIRE(std::is_nothrow_default_constructible_v<TestType>);
    REQUIRE(std::is_nothrow_copy_constructible_v<TestType>);
    REQUIRE(std::is_nothrow_move_constructible_v<TestType>);
    REQUIRE(std::is_nothrow_copy_assignable_v<TestType>);
    REQUIRE(std::is_nothrow_move_assignable_v<TestType>);
    REQUIRE(std::is_nothrow_destructible_v<TestType>);
}

TEMPLATE_TEST_CASE("RandomAccessIterator() noexcept [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    constexpr TestType it;
    REQUIRE(it == TestType{acore::INVALID_INDEX, nullptr});
}

TEMPLATE_TEST_CASE("RandomAccessIterator(size_type index, Iterable *data) noexcept [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    REQUIRE(noexcept(TestType{10, &iterable}));
}

TEMPLATE_TEST_CASE("index() const noexcept -> size_type [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    constexpr TestType it{3, nullptr};
    REQUIRE(noexcept(it.index()));
    REQUIRE(it.index() == 3);
}

TEMPLATE_TEST_CASE("iterable() const noexcept -> Iterable * [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    const TestType it{1, &iterable};
    REQUIRE(noexcept(it.iterable()));
    REQUIRE(it.iterable() == &iterable);
}

TEMPLATE_TEST_CASE("operator+(size_type index) const -> RandomAccessIterator [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    const TestType it{1, &iterable};
    const TestType other = it + 2;
    REQUIRE(other.index() == 3);
}

TEMPLATE_TEST_CASE("operator+=(size_type index) -> RandomAccessIterator & [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{1, &iterable};
    it += 3;
    REQUIRE(it.index() == 4);
}

TEMPLATE_TEST_CASE("operator++() -> RandomAccessIterator & [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{3, &iterable};
    ++it;
    REQUIRE(it.index() == 4);
}

TEMPLATE_TEST_CASE("operator++(int) & -> RandomAccessIterator [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{1, &iterable};
    const TestType other = it++;
    REQUIRE(it.index() == 2);
    REQUIRE(other.index() == 1);
}

TEMPLATE_TEST_CASE("operator-(RandomAccessIterator other) const -> difference_type [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    const TestType it{1, &iterable};
    const TestType other{3, &iterable};
    REQUIRE(std::distance(it, other) == 2);
}

TEMPLATE_TEST_CASE("operator-(size_type index) const -> RandomAccessIterator [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    const TestType it{4, &iterable};
    const TestType other = it - 3;
    REQUIRE(it.index() == 4);
    REQUIRE(other.index() == 1);
}

TEMPLATE_TEST_CASE("operator-=(size_type index) const -> RandomAccessIterator & [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{4, &iterable};
    it -= 2;
    REQUIRE(it.index() == 2);
}

TEMPLATE_TEST_CASE("operator--() -> RandomAccessIterator & [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{3, &iterable};
    --it;
    REQUIRE(it.index() == 2);
}

TEMPLATE_TEST_CASE("operator--(int) & -> RandomAccessIterator [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{1, &iterable};
    const TestType other = it--;
    REQUIRE(it.index() == 0);
    REQUIRE(other.index() == 1);
}

TEST_CASE("operator*() const -> reference [acore::RandomAccessIterator]")
{
    Iterable iterable;

    SECTION("[iterator]")
    {
        const Iterable::iterator it{2, &iterable};
        *it = 10; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(iterable.data() == std::vector<int>{1, 2, 10, 4, 5});
    }

    SECTION("[const_iterator]")
    {
        const Iterable::const_iterator it{3, &iterable};
        REQUIRE(*it == 4);
    }
}

TEST_CASE("operator[](size_type index) const -> reference [acore::RandomAccessIterator]")
{
    Iterable iterable;

    SECTION("[iterator]")
    {
        const Iterable::iterator it{2, &iterable};
        it[2] = 10; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        REQUIRE(iterable.data() == std::vector<int>{1, 2, 3, 4, 10});
    }

    SECTION("[const_iterator]")
    {
        const Iterable::const_iterator it{3, &iterable};
        REQUIRE(it[-2] == 2);
    }
}

TEMPLATE_TEST_CASE("operator<(RandomAccessIterator right) const noexcept -> bool [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{3, &iterable};

    SECTION("[less]")
    {
        TestType other{4, &iterable};
        REQUIRE(it < other);
    }

    SECTION("[equal]")
    {
        TestType other{3, &iterable};
        REQUIRE_FALSE(it < other);
    }

    SECTION("[greater]")
    {
        TestType other{2, &iterable};
        REQUIRE_FALSE(it < other);
    }
}

TEMPLATE_TEST_CASE("operator<=(RandomAccessIterator right) const noexcept -> bool [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{3, &iterable};

    SECTION("[less]")
    {
        TestType other{4, &iterable};
        REQUIRE(it <= other);
    }

    SECTION("[equal]")
    {
        TestType other{3, &iterable};
        REQUIRE(it <= other);
    }

    SECTION("[greater]")
    {
        TestType other{2, &iterable};
        REQUIRE_FALSE(it <= other);
    }
}

TEMPLATE_TEST_CASE("operator>(RandomAccessIterator right) const noexcept -> bool [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{3, &iterable};

    SECTION("[less]")
    {
        TestType other{4, &iterable};
        REQUIRE_FALSE(it > other);
    }

    SECTION("[equal]")
    {
        TestType other{3, &iterable};
        REQUIRE_FALSE(it > other);
    }

    SECTION("[greater]")
    {
        TestType other{2, &iterable};
        REQUIRE(it > other);
    }
}

TEMPLATE_TEST_CASE("operator>=(RandomAccessIterator right) const noexcept -> bool [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    TestType it{3, &iterable};

    SECTION("[less]")
    {
        TestType other{4, &iterable};
        REQUIRE_FALSE(it >= other);
    }

    SECTION("[equal]")
    {
        TestType other{3, &iterable};
        REQUIRE(it >= other);
    }

    SECTION("[greater]")
    {
        TestType other{2, &iterable};
        REQUIRE(it >= other);
    }
}

TEST_CASE("operator RandomAccessIterator<const ValueType, ConstReferenceType, const Iterable>() const noexcept [acore::RandomAccessIterator]")
{
    Iterable iterable;
    const Iterable::iterator it{1, &iterable};
    const Iterable::const_iterator other = it;
    REQUIRE(it.index() == other.index());
    REQUIRE(it.iterable() == other.iterable());
}

TEMPLATE_TEST_CASE("operator==(RandomAccessIterator<ValueType, ReferenceType, Iterable> left, RandomAccessIterator<OtherValueType, OtherReferenceType, OtherIterable> right) noexcept -> bool [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;

    SECTION("[equal]")
    {
        const TestType it{1, &iterable};
        const TestType other{1, &iterable};

        REQUIRE(noexcept(it == other));
        REQUIRE(it == other);
    }

    SECTION("[different]")
    {
        const TestType it{1, &iterable};
        constexpr TestType other{1, nullptr};

        REQUIRE_FALSE(it == other);
    }
}

TEMPLATE_TEST_CASE("operator!=(RandomAccessIterator<ValueType, ReferenceType, Iterable> left, RandomAccessIterator<OtherValueType, OtherReferenceType, OtherIterable> right) noexcept -> bool [acore::RandomAccessIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;

    SECTION("[equal]")
    {
        const TestType it{1, &iterable};
        const TestType other{1, &iterable};

        REQUIRE(noexcept(it != other));
        REQUIRE_FALSE(it != other);
    }

    SECTION("[different]")
    {
        const TestType it{1, &iterable};
        constexpr TestType other{1, nullptr};

        REQUIRE(it != other);
    }
}
}
