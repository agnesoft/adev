import atest;
import awinapi;

#ifdef _WIN32
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Pipe", [] { // NOLINT(cert-err58-cpp)
    test("type traits", [] {
        expect(std::is_default_constructible_v<::awinapi::Pipe>).to_be(true);
        expect(std::is_copy_constructible_v<::awinapi::Pipe>).to_be(false);
        expect(std::is_copy_assignable_v<::awinapi::Pipe>).to_be(false);
        expect(std::is_nothrow_move_constructible_v<::awinapi::Pipe>).to_be(true);
        expect(std::is_nothrow_move_assignable_v<::awinapi::Pipe>).to_be(true);
    });

    test("default constructed", [] {
        ::awinapi::Pipe pipe;
        expect_fail(pipe.read_handle()).to_be(nullptr);
        expect_fail(pipe.write_handle()).to_be(nullptr);
    });

    test("close read", [] {
        ::awinapi::Pipe pipe;
        pipe.close_read();
        expect(pipe.read_handle()).to_be(nullptr);
        expect_fail(pipe.write_handle()).to_be(nullptr);
    });

    test("close write", [] {
        ::awinapi::Pipe pipe;
        pipe.close_write();
        expect_fail(pipe.read_handle()).to_be(nullptr);
        expect(pipe.write_handle()).to_be(nullptr);
    });
});
#endif
