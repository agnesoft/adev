import atest;
import awinapi;

#ifdef _WIN32
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

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
        expect(buffer.tchar_size()).to_be(0U);
        expect(buffer.byte_size()).to_be(0U);
        expect(buffer.str()).to_be(nullptr);
    });

    test("constructed with size", [] {
        constexpr DWORD bufferSize = 8U;
        ::awinapi::Buffer buffer{bufferSize};
        expect(buffer.tchar_size()).to_be(bufferSize);
        expect(buffer.byte_size()).to_be(bufferSize);
        expect_fail(buffer.str()).to_be(nullptr);
    });

    test("move constructor", [] {
        constexpr DWORD bufferSize = 8U;
        ::awinapi::Buffer buffer{bufferSize};
        ::awinapi::Buffer other{std::move(buffer)};
        expect(other.tchar_size()).to_be(bufferSize);
        expect(other.byte_size()).to_be(bufferSize);
        expect_fail(other.str()).to_be(nullptr);
    });

    test("move assignment", [] {
        constexpr DWORD bufferSize = 8U;
        ::awinapi::Buffer buffer{bufferSize};
        ::awinapi::Buffer other;
        other = std::move(buffer);
        expect(other.tchar_size()).to_be(bufferSize);
        expect(other.byte_size()).to_be(bufferSize);
        expect_fail(other.str()).to_be(nullptr);
    });

    test("move assignment to itself", [] {
        constexpr DWORD bufferSize = 8U;
        ::awinapi::Buffer buffer{bufferSize};
        ::awinapi::Buffer &other = buffer;
        buffer = std::move(other);
        expect(buffer.tchar_size()).to_be(bufferSize);
        expect(buffer.byte_size()).to_be(bufferSize);
        expect_fail(buffer.str()).to_be(nullptr);
    });
});
#endif
