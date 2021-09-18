import atest;
import acommandline;

using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("CommandLine::parse()", [] {
    test("long and short mix", [] {
        ::acommandline::CommandLine commandLine;

        bool value = false;
        std::string option;
        std::int64_t another = 0;
        std::string positional;

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);
        commandLine.option().long_name("option").short_name('o').description("").bind_to(&option);
        commandLine.option().long_name("yetanother").description("").bind_to(&another);
        commandLine.option().positional().description("").bind_to(&positional);
        commandLine.parse(6, std::vector<const char *>{"./app", "-v", "-o=file", "--yetanother", "10", "somefile"}.data());

        expect(value).to_be(true);
        expect(option).to_be("file");
        expect(another).to_be(10);
        expect(positional).to_be("somefile");
    });

    test("single, missing, defaulted and repeated options", [] {
        ::acommandline::CommandLine commandLine;

        std::vector<int64_t> parameters;
        bool flag = true;
        std::string output;
        std::vector<std::string> paths;

        commandLine.option().long_name("param").short_name('p').description("").bind_to(&parameters);
        commandLine.option().long_name("flag").short_name('f').description("").bind_to(&flag);
        commandLine.option().positional().required().description("").bind_to(&output);
        commandLine.option().long_name("include").short_name('I').default_value(std::vector<std::string>{"\"c:/path/with spaces/", "//"}).description("").bind_to(&paths);
        commandLine.parse(6, std::vector<const char *>{"a", "-p=1", "-p=2", "--param", "100", "output_option"}.data());

        expect(parameters).to_be(std::vector<std::int64_t>{1, 2, 100});
        expect(flag).to_be(true);
        expect(output).to_be("output_option");
        expect(paths).to_be(std::vector<std::string>{"\"c:/path/with spaces/", "//"});
    });

    test("no arguments", [] {
        ::acommandline::CommandLine commandLine;

        expect([&] {
            commandLine.parse(0, std::vector<const char *>{}.data());
        })
            .to_throw<std::logic_error>("Missing mandatory first command line argument.");
    });

    test("unmatched argument", [] {
        ::acommandline::CommandLine commandLine;

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data());
        })
            .to_throw<std::runtime_error>("Unknown option '-v'.");
    });
});
