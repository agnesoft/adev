import atest;
import acommandline;

using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("required", [] {
    test("unmatched positional", [] {
        ::acommandline::CommandLine commandLine;

        std::vector<std::string> value;
        commandLine.option().positional().required().description("").bind_to(&value);

        expect([&] {
            commandLine.parse(1, std::vector<const char *>{"app"}.data());
        })
            .to_throw<std::runtime_error>("Option '[positional]' was set as required but did not match any arguments.");
    });

    test("unmatched named", [] {
        ::acommandline::CommandLine commandLine;

        bool value = false;
        double required = 0.0;

        commandLine.option().long_name("long_name").short_name('v').description("").bind_to(&value);
        commandLine.option().positional().required().description("").bind_to(&required);

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data());
        })
            .to_throw<std::runtime_error>("Option '[positional]' was set as required but did not match any arguments.");
    });
});
