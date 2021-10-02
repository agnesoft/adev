import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("CommandLine::command()", [] { // NOLINT(cert-err58-cpp)
    test("./app.exe", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        commandLine.parse(1, std::vector<const char *>{"./app.exe"}.data());
        expect(commandLine.command()).to_be("./app.exe");
    });

    test("./app.a", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        commandLine.parse(1, std::vector<const char *>{"./app.a"}.data());
        expect(commandLine.command()).to_be("./app.a");
    });

    test("./app", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        commandLine.parse(1, std::vector<const char *>{"./app"}.data());
        expect(commandLine.command()).to_be("./app");
    });

    test("app.exe", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        commandLine.parse(1, std::vector<const char *>{"app.exe"}.data());
        expect(commandLine.command()).to_be("app.exe");
    });

    test("app", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        commandLine.parse(1, std::vector<const char *>{"app"}.data());
        expect(commandLine.command()).to_be("app");
    });
});
