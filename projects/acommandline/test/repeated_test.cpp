import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("repeated", [] {
    test("single named", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<std::string> values;

        commandLine.option().long_name("name1").description("").bind_to(&values);
        commandLine.parse(2, std::array<const char *, 2>{"./app", "--name1=val"}.data());

        expect(values).to_be(std::vector<std::string>{"val"});
    });

    test("named", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<double> values;

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&values);
        commandLine.parse(7, std::array<const char *, 7>{"./app", "-v", "33.3", "--value=51.61234", "-v=9.999", "-v", "19.19"}.data());

        expect(values).to_be(std::vector{33.3, 51.61234, 9.999, 19.19});
    });

    test("single positional", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<double> values;

        commandLine.option().positional().description("").bind_to(&values);
        commandLine.parse(2, std::array<const char *, 2>{"./app", "13.50"}.data());

        expect(values).to_be(std::vector{13.50});
    });

    test("positional", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<std::string> values;
        commandLine.option().positional().default_value(std::vector<std::string>{"input"}).description("").bind_to(&values);
        commandLine.parse(4, std::array<const char *, 4>{"./app", "input1", "input2", "input3"}.data());

        expect(values).to_be(std::vector<std::string>{"input1", "input2", "input3"});
    });
});
