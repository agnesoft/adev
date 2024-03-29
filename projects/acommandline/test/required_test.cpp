import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("required", [] { //NOLINT(cert-err58-cpp)
    test("matched positional", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::string value;
        commandLine.option().positional().required().description("").bind_to(&value);
        commandLine.parse(2, std::array<const char *, 2>{"app", "req"}.data());

        expect(value).to_be("req");
    });

    test("unmatched positional", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<std::string> value;
        commandLine.option().positional().required().description("").bind_to(&value);

        expect([&] {
            commandLine.parse(1, std::array<const char *, 1>{"app"}.data());
        })
            .to_throw<std::runtime_error>("Option '[positional]' was set as required but did not match any arguments.");
    });

    test("unmatched named", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        bool value = false;
        double required = 0.0;

        commandLine.option().long_name("long_name").short_name('v').description("").bind_to(&value);
        commandLine.option().positional().required().description("").bind_to(&required);

        expect([&] {
            commandLine.parse(2, std::array<const char *, 2>{"./app", "-v"}.data());
        })
            .to_throw<std::runtime_error>("Option '[positional]' was set as required but did not match any arguments.");
    });
});
