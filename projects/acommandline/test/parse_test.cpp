import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("CommandLine::parse()", [] { //NOLINT(cert-err58-cpp)
    test("long and short mix", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        bool value = false;
        std::string option;
        std::int64_t another = 0;
        std::string positional;
        constexpr int argc = 6;

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);
        commandLine.option().long_name("option").short_name('o').description("").bind_to(&option);
        commandLine.option().long_name("yetanother").description("").bind_to(&another);
        commandLine.option().positional().description("").bind_to(&positional);
        commandLine.parse(argc, std::array<const char *, argc>{"./app", "-v", "-o=file", "--yetanother", "1", "somefile"}.data());

        expect(value).to_be(true);
        expect(option).to_be("file");
        expect(another).to_be(1);
        expect(positional).to_be("somefile");
    });

    test("quoted", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::string value;

        commandLine.option().long_name("value").description("").bind_to(&value);
        commandLine.parse(3, std::array<const char *, 3>{"./app", "--value", "\"my long quoted value\""}.data());

        expect(value).to_be("my long quoted value");
    });

    test("single, missing, defaulted and repeated options", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::vector<int64_t> parameters;
        bool flag = true;
        std::string output;
        std::vector<std::string> paths;
        constexpr int argc = 6;

        commandLine.option().long_name("param").short_name('p').description("").bind_to(&parameters);
        commandLine.option().long_name("flag").short_name('f').description("").bind_to(&flag);
        commandLine.option().positional().required().description("").bind_to(&output);
        commandLine.option().long_name("include").short_name('I').default_value(std::vector<std::string>{"c:/path/with spaces/", "//"}).description("").bind_to(&paths);
        commandLine.parse(argc, std::array<const char *, argc>{"a", "-p=1", "-p=2", "--param", "3", "output_option"}.data());

        expect(parameters).to_be(std::vector<std::int64_t>{1, 2, 3});
        expect(flag).to_be(true);
        expect(output).to_be("output_option");
        expect(paths).to_be(std::vector<std::string>{"c:/path/with spaces/", "//"});
    });

    test("no arguments", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        expect([&] {
            commandLine.parse(0, std::array<const char *, 0>{}.data());
        })
            .to_throw<std::logic_error>("Missing mandatory first command line argument.");
    });

    test("unmatched argument", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        expect([&] {
            commandLine.parse(2, std::array<const char *, 2>{"./app", "-v"}.data());
        })
            .to_throw<std::runtime_error>("Unmatched argument '-v'.");
    });
});
