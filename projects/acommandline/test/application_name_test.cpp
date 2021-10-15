import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("CommandLine::application_name()", [] { // NOLINT(cert-err58-cpp)
    test("./app.exe", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        expect(commandLine.parse(1, std::vector<const char *>{"./app.exe"}.data())).to_be(true);
        expect(commandLine.application_name()).to_be("app");
    });

    test("./app.a", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        expect(commandLine.parse(1, std::vector<const char *>{"./app.a"}.data())).to_be(true);
        expect(commandLine.application_name()).to_be("app");
    });

    test("./app", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        expect(commandLine.parse(1, std::vector<const char *>{"./app"}.data())).to_be(true);
        expect(commandLine.application_name()).to_be("app");
    });

    test("app.exe", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        expect(commandLine.parse(1, std::vector<const char *>{"app.exe"}.data())).to_be(true);
        expect(commandLine.application_name()).to_be("app");
    });

    test("app", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        expect(commandLine.parse(1, std::vector<const char *>{"app"}.data())).to_be(true);
        expect(commandLine.application_name()).to_be("app");
    });
});
