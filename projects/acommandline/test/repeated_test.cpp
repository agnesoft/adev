import atest;
import acommandline;

using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("repeated", [] {
    test("named", [] {
        ::acommandline::CommandLine commandLine;

        std::vector<double> values;

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&values);
        commandLine.parse(7, std::vector<const char *>{"./app", "-v", "33.3", "--value=51.61234", "-v=9.999", "-v", "19.19"}.data());

        expect(values).to_be(std::vector{33.3, 51.61234, 9.999, 19.19});
    });

    test("positional", [] {
        ::acommandline::CommandLine commandLine;

        std::vector<std::string> values;
        commandLine.option().positional().default_value(std::vector<std::string>{"input"}).description("").bind_to(&values);
        commandLine.parse(4, std::vector<const char *>{"./app", "input1", "input2", "input3"}.data());

        expect(values).to_be(std::vector<std::string>{"input1", "input2", "input3"});
    });
});
