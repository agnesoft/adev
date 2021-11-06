import atest;
import awinapi;

#ifdef _WIN32
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("last_error_message()", [] { // NOLINT(cert-err58-cpp)
    test("no error", [] {
        ::SetLastError(0U);
        expect(::awinapi::last_error_message()).to_be(std::string{});
    });

    test("invalid handle error", [] {
        ::CloseHandle(nullptr);
        const std::string errorMessage = ::awinapi::last_error_message();
        expect(errorMessage.size() > 4).to_be(true);
        expect(errorMessage.substr(errorMessage.size() - 4)).to_be(" (6)");
    });
});
#endif
