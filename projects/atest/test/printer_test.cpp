import atest;

using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

struct MyStruct
{
    int value = 0; // NOLINT(misc-non-private-member-variables-in-classes)

    [[nodiscard]] auto operator==(const MyStruct &other) const noexcept -> bool
    {
        return value == other.value;
    }
};

auto operator<<(std::ostream &stream, const MyStruct &value) -> std::ostream &
{
    stream << "MyStruct{" << value.value << '}';
    return stream;
}

static const auto S = suite("Printer", [] { // NOLINT(cert-err58-cpp)
    test("Print custom type", [] {
        expect_fail(MyStruct{1}).to_be(MyStruct{2});
    });

    test("Print container", [] {
        expect_fail(std::vector<int>{1, 2, 3}).to_be(std::vector<int>{3, 2, 1});
    });

    test("Print container (INTENTIONAL FAILURE)", [] {
        expect(std::vector<int>{1, 2, 3}).to_be(std::vector<int>{3, 2, 1});
    });

    test("Print container of custom type", [] {
        expect_fail(std::vector<MyStruct>{{1}, {2}, {3}}).to_be(std::vector<MyStruct>{{3}, {2}, {1}});
    });

    test("Print container of custom type (INTENTIONAL FAILURE)", [] {
        expect(std::vector<MyStruct>{{1}, {2}, {3}}).to_be(std::vector<MyStruct>{{3}, {2}, {1}});
    });

    test("Print std::pair (INTENTIONAL FAILURE)", [] {
        expect(std::pair<int, int>{1, 2}).to_be(std::pair<int, int>{2, 1});
    });
});
