import atest;
import awinapi;

#ifdef _WIN32
using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("Handle", [] { // NOLINT(cert-err58-cpp)
    test("type traits", [] {
        expect(std::is_nothrow_default_constructible_v<::awinapi::Handle>).to_be(true);
        expect(std::is_copy_constructible_v<::awinapi::Handle>).to_be(false);
        expect(std::is_copy_assignable_v<::awinapi::Handle>).to_be(false);
        expect(std::is_nothrow_move_constructible_v<::awinapi::Handle>).to_be(true);
        expect(std::is_nothrow_move_assignable_v<::awinapi::Handle>).to_be(true);
    });

    test("default constructed", [] {
        ::awinapi::Handle handle;
        expect(handle.get()).to_be(nullptr);
    });

    test("constructed with a handle", [] {
        int value = 1;
        ::HANDLE handle = static_cast<::HANDLE>(&value);
        ::awinapi::Handle wrapper{handle};
        expect(wrapper.get()).to_be(handle);
        wrapper.get() = nullptr;
    });

    test("assign a handle", [] {
        int value = 1;
        ::HANDLE handle = static_cast<::HANDLE>(&value);
        ::awinapi::Handle wrapper;
        assert_(wrapper.get()).to_be(nullptr);
        wrapper.get() = handle;
        expect(wrapper.get()).to_be(handle);
        wrapper.get() = nullptr;
    });

    test("move constructor", [] {
        int value = 1;
        ::HANDLE handle = static_cast<::HANDLE>(&value);
        ::awinapi::Handle wrapper{handle};
        ::awinapi::Handle other{std::move(wrapper)};
        expect(other.get()).to_be(handle);
        other.get() = nullptr;
    });

    test("move assignment", [] {
        int value = 1;
        ::HANDLE handle = static_cast<::HANDLE>(&value);
        ::awinapi::Handle wrapper{handle};
        ::awinapi::Handle other;
        other = std::move(wrapper);
        expect(other.get()).to_be(handle);
        other.get() = nullptr;
    });
});
#endif
