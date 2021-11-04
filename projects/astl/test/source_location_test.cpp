import atest;
import astl;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("source_location", [] { // NOLINT(cert-err58-cpp)
    test("default constructed", [] {
        std::source_location location;
        std::string fileName = location.file_name();

        expect(fileName).to_be("");
        expect(location.line()).to_be(0u);
    });

    test("current()", [] {
        std::source_location location = std::source_location::current();
        std::string fileName = location.file_name();

        expect(fileName.ends_with("source_location_test.cpp")).to_be(true);
        expect(location.line()).to_be(19u);
    });
});
