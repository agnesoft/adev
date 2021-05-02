import atest;

using atest::assert_;
using atest::assert_fail;
using atest::expect;
using atest::expect_fail;
using atest::suite;
using atest::test;

struct S
{
    int value = 0; //NOLINT(misc-non-private-member-variables-in-classes)

    [[nodiscard]] auto operator==(const S &other) const noexcept -> bool
    {
        return value == other.value;
    }
};

auto operator<<(std::ostream &stream, const S &value) -> std::ostream &
{
    stream << "S{" << value.value << '}';
    return stream;
}

static const auto s = suite("Printer", [] { //NOLINT(cert-err58-cpp)
    test("Print custom type", [] {
        expect_fail(S{1}).toBe(S{2});
    });

    test("Print container", [] {
        expect_fail(std::vector<int>{1, 2, 3}).toBe(std::vector<int>{3, 2, 1});
    });

    test("Print container (INTENTIONAL FAILURE)", [] {
        expect(std::vector<int>{1, 2, 3}).toBe(std::vector<int>{3, 2, 1});
    });

    test("Print container of custom type", [] {
        expect_fail(std::vector<S>{{1}, {2}, {3}}).toBe(std::vector<S>{{3}, {2}, {1}});
    });

    test("Print container of custom type (INTENTIONAL FAILURE)", [] {
        expect(std::vector<S>{{1}, {2}, {3}}).toBe(std::vector<S>{{3}, {2}, {1}});
    });
});
