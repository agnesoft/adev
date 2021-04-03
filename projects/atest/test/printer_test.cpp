import atest;

using atest::expect;
using atest::suite;
using atest::test;

struct S
{
    int value = 0;

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
    test("custom type", [] {
        expect(S{1}).toBe(S{2}).toFail();
    });

    test("container", [] {
        expect(std::vector<int>{1, 2, 3}).toBe(std::vector<int>{3, 2, 1}).toFail();
    });

    test("container of custom type", [] {
        expect(std::vector<S>{{1}, {2}, {3}}).toBe(std::vector<S>{{3}, {2}, {1}}).toFail();
    });
});
