import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("repeated", [] { // NOLINT(cert-err58-cpp)
    test("single named", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<std::string> values;

        commandLine.option().long_name("name1").description("").bind_to(&values);
        expect(commandLine.parse(2, std::array<const char *, 2>{"./app", "--name1=val"}.data())).to_be(true);

        expect(values).to_be(std::vector<std::string>{"val"});
    });

    test("named", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<double> values;
        constexpr int argc = 7;

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&values);
        expect(commandLine.parse(argc, std::array<const char *, argc>{"./app", "-v", "33.3", "--value=51.61234", "-v=9.999", "-v", "19.19"}.data())).to_be(true);

        expect(values).to_be(std::vector{33.3, 51.61234, 9.999, 19.19}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    });

    test("single positional", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<std::int64_t> values;

        commandLine.option().positional().description("").bind_to(&values);
        expect(commandLine.parse(2, std::array<const char *, 2>{"./app", "1350"}.data())).to_be(true);

        expect(values).to_be(std::vector<std::int64_t>{1350}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    });

    test("positional", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<std::string> values;
        commandLine.option().positional().default_value(std::vector<std::string>{"input"}).description("").bind_to(&values);
        expect(commandLine.parse(4, std::array<const char *, 4>{"./app", "input1", "input2", "input3"}.data())).to_be(true);

        expect(values).to_be(std::vector<std::string>{"input1", "input2", "input3"});
    });
});
