import atest;

using atest::expect;
using atest::suite;
using atest::test;

struct S
{
    int value = 0;
};

[[nodiscard]] auto operator==(const S &left, const S &right) noexcept -> bool
{
    return left.value == right.value;
}

auto operator<<(std::ostream &stream, const S &value) -> std::ostream &
{
    stream << "S{" << value.value << '}';
    return stream;
}

static const auto s = suite("Printer", [] { //NOLINT(cert-err58-cpp)
    test("custom type", [] {
        expect(S{1}).toBe(S{2}).toFail();
    });
});

