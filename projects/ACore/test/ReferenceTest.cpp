#include "pch.hpp"

#include <catch2/catch.hpp>

#include <filesystem>
#include <fstream>
#include <vector>

namespace referencetest
{
class Container
{
public:
    [[nodiscard]] auto data() const noexcept -> const std::vector<int> &
    {
        return mData;
    }

    [[nodiscard]] auto value(acore::size_type index) const -> int
    {
        return mData[index];
    }

    auto setValue(acore::size_type index, int value) -> void
    {
        mData[index] = value;
    }

private:
    std::vector<int> mData = {1, 2, 3, 4, 5}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
};

TEST_CASE("[acore::Reference]")
{
    REQUIRE_FALSE(std::is_default_constructible_v<acore::Reference<int, Container>>);
    REQUIRE(std::is_nothrow_copy_constructible_v<acore::Reference<int, Container>>);
    REQUIRE(std::is_nothrow_move_constructible_v<acore::Reference<int, Container>>);
    REQUIRE(std::is_copy_assignable_v<acore::Reference<int, Container>>);
    REQUIRE(std::is_nothrow_move_assignable_v<acore::Reference<int, Container>>);
    REQUIRE(std::is_nothrow_destructible_v<acore::Reference<int, Container>>);
}

TEST_CASE("Reference(size_type index, Data *data) noexcept [acore::Reference]")
{
    Container container;
    REQUIRE(noexcept(acore::Reference<int, Container>{2, &container}));
}

TEST_CASE("Reference(const Reference &other) noexcept [acore::Reference]")
{
    Container container;
    const acore::Reference<int, Container> ref{1, &container};
    REQUIRE(noexcept(acore::Reference<int, Container>{ref}));
    const acore::Reference<int, Container> other{ref};
    REQUIRE(ref == other);
}

TEST_CASE("Reference(Reference &&other) noexcept [acore::Reference]")
{
    Container container;
    acore::Reference<int, Container> ref{3, &container};
    REQUIRE(noexcept(acore::Reference<int, Container>{std::move(ref)})); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    const acore::Reference<int, Container> other{std::move(ref)}; //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    REQUIRE(other == 4);
}

TEST_CASE("operator T() const [acore::Reference]")
{
    Container container;
    const acore::Reference<int, Container> ref{0, &container};
    const int value = ref;
    REQUIRE(value == 1);
}

TEST_CASE("operator*() const -> T [acore::Reference]")
{
    Container container;
    const acore::Reference<int, Container> ref{4, &container};
    REQUIRE(*ref == 5);
}

TEST_CASE("operator=(T value) -> Referece & [acore::Reference]")
{
    Container container;
    acore::Reference<int, Container> ref{4, &container};
    ref = -2;
    REQUIRE(container.data() == std::vector<int>{1, 2, 3, 4, -2});
}

TEST_CASE("operator=(const Reference &other) -> Reference & [acore::Reference]")
{
    Container container;
    acore::Reference<int, Container> ref{1, &container};
    acore::Reference<int, Container> other{3, &container};
    ref = other;
    REQUIRE(container.data() == std::vector<int>{1, 4, 3, 4, 5});
}

TEST_CASE("operator=(Reference &&other) noexcept -> Reference & [acore::Reference]")
{
    Container container;
    acore::Reference<int, Container> ref{1, &container};
    acore::Reference<int, Container> other{3, &container};
    REQUIRE(noexcept(ref = std::move(other))); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    ref = std::move(other); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    REQUIRE(container.data() == std::vector<int>{1, 4, 3, 4, 5});
}

TEST_CASE("operator==(Reference<T, Data> reference, const T &value) -> bool [acore::Reference]")
{
    Container container;
    acore::Reference<int, Container> ref{2, &container};

    SECTION("[equal]")
    {
        REQUIRE(ref == 3);
    }

    SECTION("[different]")
    {
        REQUIRE_FALSE(ref == 10);
    }
}

TEST_CASE("operator!=(Reference<T, Data> reference, const T &value) -> bool [acore::Reference]")
{
    Container container;
    acore::Reference<int, Container> ref{2, &container};

    SECTION("[equal]")
    {
        REQUIRE_FALSE(ref != 3);
    }

    SECTION("[different]")
    {
        REQUIRE(ref != 10);
    }
}

TEST_CASE("operator==(Reference<T, Data> left, Reference<T, Data> right) -> bool [acore::Reference]")
{
    Container container;
    acore::Reference<int, Container> ref{3, &container};

    SECTION("[equal]")
    {
        REQUIRE(ref == acore::Reference<int, Container>{3, &container});
    }

    SECTION("[different]")
    {
        REQUIRE_FALSE(ref == acore::Reference<int, Container>{1, &container});
    }
}

TEST_CASE("operator!=(Reference<T, Data> left, Reference<T, Data> right) -> bool [acore::Reference]")
{
    Container container;
    acore::Reference<int, Container> ref{2, &container};

    SECTION("[equal]")
    {
        REQUIRE_FALSE(ref != acore::Reference<int, Container>{2, &container});
    }

    SECTION("[different]")
    {
        REQUIRE(ref != acore::Reference<int, Container>{1, &container});
    }
}

TEST_CASE("acore::Reference [examples]")
{
    SECTION("[usage]")
    {
        std::remove("acore_reference_example.testfile");
        // clang-format off
//! [[usage]]
class FileData
{
public:
    auto setValue(std::int64_t index, int value) -> void
    {
        file.seekg(static_cast<size_t>(index));
        file << value;
    }

    auto value(std::int64_t index) -> int
    {
        int v = 0;
        file.seekg(static_cast<size_t>(index));
        file >> v;
        return v;
    }

private:
    std::fstream file = std::fstream("acore_reference_example.testfile", std::fstream::binary | std::fstream::trunc | std::fstream::in | std::fstream::out); //NOLINT(hicpp-signed-bitwise)
};

FileData data;
acore::Reference<int, FileData> ref{3, &data};
ref = 4; //writes 4 to file at position 3
int i = ref; //reads 10 from file at poition 3
//! [[usage]]
        // clang-format on
        REQUIRE(ref == 4);
        REQUIRE(i == 4);
        REQUIRE(i == ref);
    }

    REQUIRE(std::filesystem::remove("acore_reference_example.testfile"));
}
}
