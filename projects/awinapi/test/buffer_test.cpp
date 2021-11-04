import atest;
import awinapi;

using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

#ifdef _WIN32
static const auto S = suite("Buffer", [] { // NOLINT(cert-err58-cpp)
    test("type traits", [] {
        expect(std::is_nothrow_default_constructible_v<::awinapi::Buffer>).to_be(true);
        expect(std::is_copy_constructible_v<::awinapi::Buffer>).to_be(false);
        expect(std::is_copy_assignable_v<::awinapi::Buffer>).to_be(false);
        expect(std::is_nothrow_move_constructible_v<::awinapi::Buffer>).to_be(true);
        expect(std::is_nothrow_move_assignable_v<::awinapi::Buffer>).to_be(true);
    });

    test("default constructed", [] {
        ::awinapi::Buffer buffer;
        expect(buffer.tchar_size()).to_be(0u);
        expect(buffer.byte_size()).to_be(0u);
        expect(buffer.str()).to_be(nullptr);
    });

    test("constructed with size", [] {
        ::awinapi::Buffer buffer{8u};
        expect(buffer.tchar_size()).to_be(8u);
        expect(buffer.byte_size()).to_be(8u);
        expect_fail(buffer.str()).to_be(nullptr);
    });

    test("move constructor", [] {
        ::awinapi::Buffer buffer{8u};
        ::awinapi::Buffer other{std::move(buffer)};
        expect(other.tchar_size()).to_be(8u);
        expect(other.byte_size()).to_be(8u);
        expect_fail(other.str()).to_be(nullptr);
    });

    test("move assignment", [] {
        ::awinapi::Buffer buffer{8u};
        ::awinapi::Buffer other;
        other = std::move(buffer);
        expect(other.tchar_size()).to_be(8u);
        expect(other.byte_size()).to_be(8u);
        expect_fail(other.str()).to_be(nullptr);
    });
});
#endif
