import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("CommandLine:application_name()", [] {
    test("./app.exe", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"./app.exe"}.data());
        expect(commandLine.application_name()).to_be("app");
    });

    test("./app.a", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"./app.a"}.data());
        expect(commandLine.application_name()).to_be("app");
    });

    test("./app", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"./app"}.data());
        expect(commandLine.application_name()).to_be("app");
    });

    test("app.exe", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"app.exe"}.data());
        expect(commandLine.application_name()).to_be("app");
    });

    test("app", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"app"}.data());
        expect(commandLine.application_name()).to_be("app");
    });
});
