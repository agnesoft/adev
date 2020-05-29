#include "pch.hpp"

#include <catch2/catch.hpp>

#include <sstream>

namespace forwarditeratortest
{
class Iterable
{
public:
    using iterator = acore::ForwardIterator<int, int &, Iterable>;
    using const_iterator = acore::ForwardIterator<int, int, const Iterable>;

    [[nodiscard]] auto referenceAt(acore::size_type index) -> int &
    {
        return mData[static_cast<size_t>(index)];
    }

    [[nodiscard]] auto referenceAt(acore::size_type index) const -> int
    {
        return mData[static_cast<size_t>(index)];
    }

    [[nodiscard]] auto nextIndex(acore::size_type index) const noexcept -> acore::size_type
    {
        if (index < static_cast<acore::size_type>(mData.size() - 1))
        {
            return ++index;
        }

        return acore::INVALID_INDEX;
    }

    [[nodiscard]] auto data() const noexcept -> const std::vector<int> &
    {
        return mData;
    }

private:
    std::vector<int> mData = {1, 2, 3, 4, 5}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
};
}

namespace Catch
{
template<>
struct StringMaker<forwarditeratortest::Iterable::iterator>
{
    [[nodiscard]] static auto convert(const forwarditeratortest::Iterable::iterator &it) -> std::string
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
struct StringMaker<forwarditeratortest::Iterable::const_iterator>
{
    [[nodiscard]] static auto convert(const forwarditeratortest::Iterable::const_iterator &it) -> std::string
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

namespace forwarditeratortest
{
TEMPLATE_TEST_CASE("ForwardIterator() noexcept [acore::ForwardIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    REQUIRE(noexcept(TestType{}));
    REQUIRE(TestType{} == TestType{acore::INVALID_INDEX, nullptr});
}

TEMPLATE_TEST_CASE("ForwardIterator(size_type index, Iterable *iterable) noexcept [acore::ForwardIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    REQUIRE(noexcept(TestType{5, &iterable}));
}

TEMPLATE_TEST_CASE("index() const noexcept -> size_type [acore::ForwardIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    const TestType it{3, &iterable};
    REQUIRE(noexcept(it.index()));
    REQUIRE(it.index() == 3);
}

TEMPLATE_TEST_CASE("iterable() const noexcept -> Iterable * [acore::ForwardIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;
    const TestType it{3, &iterable};
    REQUIRE(noexcept(it.iterable()));
    REQUIRE(it.iterable() == &iterable);
}

TEMPLATE_TEST_CASE("operator++() -> ForwardIterator & [acore::ForwardIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;

    SECTION("[in bounds]")
    {
        TestType it{3, &iterable};
        ++it;
        REQUIRE(it.index() == 4);
    }

    SECTION("[out of bounds]")
    {
        TestType it{7, &iterable}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        ++it;
        REQUIRE(it.index() == acore::INVALID_INDEX);
    }
}

TEMPLATE_TEST_CASE("operator++(int) & -> ForwardIterator & [acore::ForwardIterator]", "", Iterable::iterator, Iterable::const_iterator)
{
    Iterable iterable;

    SECTION("[in bounds]")
    {
        TestType it{3, &iterable};
        TestType other = it++;
        REQUIRE(other.index() == 3);
        REQUIRE(it.index() == 4);
    }

    SECTION("[out of bounds]")
    {
        TestType it{7, &iterable}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        TestType other = it++;
        REQUIRE(other.index() == 7);
        REQUIRE(it.index() == acore::INVALID_INDEX);
    }
}

TEST_CASE("operator*() const -> reference [acore::ForwardIterator]")
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

TEST_CASE("operator ForwardIterator<ValueType, ConstReferenceT, const Iterable>() const noexcept")
{
    Iterable iterable;
    const Iterable::iterator it{1, &iterable};
    const Iterable::const_iterator other = it;
    REQUIRE(it.index() == other.index());
    REQUIRE(it.iterable() == other.iterable());
}

TEMPLATE_TEST_CASE("operator==(ForwardIterator<T, ReferenceT, Iterable> left, ForwardIterator<T, ReferenceT, Iterable> right) noexcept -> bool [acore::ForwardIterator]", "", Iterable::iterator, Iterable::const_iterator)
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

TEMPLATE_TEST_CASE("operator!=(ForwardIterator<T, ReferenceT, Iterable> left, ForwardIterator<T, ReferenceT, Iterable> right) noexcept -> bool [acore::ForwardIterator]", "", Iterable::iterator, Iterable::const_iterator)
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
