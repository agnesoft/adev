import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("CommandLine:command()", [] {
    test("./app.exe", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"./app.exe"}.data());
        expect(commandLine.command()).to_be("./app.exe");
    });

    test("./app.a", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"./app.a"}.data());
        expect(commandLine.command()).to_be("./app.a");
    });

    test("./app", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"./app"}.data());
        expect(commandLine.command()).to_be("./app");
    });

    test("app.exe", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"app.exe"}.data());
        expect(commandLine.command()).to_be("app.exe");
    });

    test("app", [] {
        ::acommandline::CommandLine commandLine;
        commandLine.parse(1, std::vector<const char *>{"app"}.data());
        expect(commandLine.command()).to_be("app");
    });
});
