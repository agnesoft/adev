import atest;
import acore;

using atest::expect;
using atest::suite;
using atest::test;

static const auto s = suite("acore::CommandLine", [] {
#ifdef _MSC_VER
    using ::type_info;
#endif

    test("type traits", [] {
        expect(std::is_default_constructible_v<acore::CommandLine>).toBe(true);
        expect(std::is_copy_constructible_v<acore::CommandLine>).toBe(true);
        expect(std::is_nothrow_move_constructible_v<acore::CommandLine>).toBe(true);
        expect(std::is_copy_assignable_v<acore::CommandLine>).toBe(true);
        expect(std::is_nothrow_move_assignable_v<acore::CommandLine>).toBe(true);
        expect(std::is_nothrow_destructible_v<acore::CommandLine>).toBe(true);
    });

    test("constructor", [] {
        acore::CommandLine{};
    });

    test("copy constructor", [] {
        std::int64_t value = 0;
        acore::CommandLine commandLine;
        commandLine.option().positional().description("").bindTo(&value);
        acore::CommandLine other{commandLine};
        other.parse(2, std::vector<const char *>{"./app.exe", "10"}.data());

        expect(value).toBe(10);

        commandLine.parse(2, std::vector<const char *>{"./diff", "100"}.data());

        expect(value).toBe(100);
    });

    test("move constructor", [] {
        std::int64_t value = 0;
        acore::CommandLine commandLine;
        commandLine.option().positional().description("").bindTo(&value);
        acore::CommandLine other{std::move(commandLine)};
        other.parse(2, std::vector<const char *>{"./app.exe", "10"}.data());

        expect(value).toBe(10);
    });

    test("applicationName", [] {
        acore::CommandLine commandLine;

        commandLine.parse(1, std::vector<const char *>{"./app.exe"}.data());
        expect(commandLine.applicationName()).toBe("app");

        commandLine.parse(1, std::vector<const char *>{"./app.a"}.data());
        expect(commandLine.applicationName()).toBe("app");

        commandLine.parse(1, std::vector<const char *>{"./app"}.data());
        expect(commandLine.applicationName()).toBe("app");

        commandLine.parse(1, std::vector<const char *>{"app.exe"}.data());
        expect(commandLine.applicationName()).toBe("app");

        commandLine.parse(1, std::vector<const char *>{"app"}.data());
        expect(commandLine.applicationName()).toBe("app");
    });

    test("command", [] {
        acore::CommandLine commandLine;

        commandLine.parse(1, std::vector<const char *>{"./app.exe"}.data());
        expect(commandLine.command()).toBe("./app.exe");

        commandLine.parse(1, std::vector<const char *>{"./app.a"}.data());
        expect(commandLine.command()).toBe("./app.a");

        commandLine.parse(1, std::vector<const char *>{"./app"}.data());
        expect(commandLine.command()).toBe("./app");

        commandLine.parse(1, std::vector<const char *>{"app.exe"}.data());
        expect(commandLine.command()).toBe("app.exe");

        commandLine.parse(1, std::vector<const char *>{"app"}.data());
        expect(commandLine.command()).toBe("app");
    });

    test("disableHelp", [] {
        acore::CommandLine commandLine;
        commandLine.disableHelp();
        expect(commandLine.helpEnabled()).toBe(false);
    });

    test("enableHelp", [] {
        acore::CommandLine commandLine;

        commandLine.disableHelp();
        expect(commandLine.helpEnabled()).toBe(false);

        commandLine.enableHelp();
        expect(commandLine.helpEnabled()).toBe(true);
    });

    test("helpDisplayed() const noexcept -> bool", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;

        commandLine.setPrintStream(&stream);
        expect(commandLine.helpDisplayed()).toBe(false);

        commandLine.parse(2, std::vector<const char *>{"app", "-?"}.data());
        expect(commandLine.helpDisplayed()).toBe(true);
    });

    test("helpEnabled() const noexcept -> bool", [] {
        acore::CommandLine commandLine;
        expect(commandLine.helpEnabled()).toBe(true);
    });

    test("option() -> CommandLineOption &", [] {
        acore::CommandLine commandLine;
        static_cast<void>(commandLine.option());
    });

    test("parse (help)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.disableHelp();

        bool b = false;
        std::int64_t i = 10;
        std::string str;
        double d{};

        commandLine.setPrintStream(&stream);
        commandLine.option().longName("longName1").shortName('l').description("Some long name argument.").bindTo(&b);
        commandLine.option().longName("another").defaultValue(10.0).description("Another argument description.").bindTo(&d);
        commandLine.option().positional().required().description("Positional argument description.").bindTo(&str);
        commandLine.option().longName("output").shortName('o').description("Output parameter.").bindTo(&i);
        commandLine.parse(2, std::vector<const char *>{"app", "-?"}.data());

        const std::string expected = "Usage:\n"
                                     "    app [options]\n"
                                     "Syntax:\n"
                                     "  switch       --switch, -s\n"
                                     "  named        --name=value, --n=value, --name value, --n value\n"
                                     "  positional   value\n"
                                     "Options:\n"
                                     "    -?                  [switch]          Prints this help.\n"
                                     "  Required:\n"
                                     "    [positional]        [string]          Positional argument description.\n"
                                     "  Optional:\n"
                                     "    -l, --longName1     [switch]          Some long name argument.\n"
                                     "    --another           [floating=10]     Another argument description.\n"
                                     "    -o, --output        [integer]         Output parameter.\n"
                                     "  Positional:\n"
                                     "    [positional]        [string]          Positional argument description.\n";

        expect(stream.str()).toBe(expected);
    });

    test("parse (default help)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;

        std::cout.setstate(std::ios_base::failbit);
        commandLine.parse(2, std::vector<const char *>{"app", "-?"}.data());
        std::cout.clear();
    });

    test("parse (single positional option)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        std::string value;

        commandLine.option().positional().description("").bindTo(&value);
        commandLine.parse(2, std::vector<const char *>{"./app.exe", "value"}.data());

        expect(value).toBe("value");
    });

    test("parse (multiple positional options)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        std::string sValue;
        std::int64_t iValue = 0;
        double dValue = 0.0;

        commandLine.option().positional().description("").bindTo(&iValue);
        commandLine.option().positional().description("").bindTo(&dValue);
        commandLine.option().positional().description("").bindTo(&sValue);
        commandLine.parse(4, std::vector<const char *>{"./app", "value", "-10", "5.5"}.data());

        expect(sValue).toBe("value");
        expect(iValue).toBe(-10);
        expect(dValue).toBe(5.5);
    });

    test("parse (single long name option)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        bool value = false;

        commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "--value"}.data());

        expect(value).toBe(true);
    });

    test("parse (multiple long name options)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        bool value = false;
        std::string option;
        std::int64_t another = 0;

        commandLine.option().longName("value").description("").bindTo(&value);
        commandLine.option().longName("option").description("").bindTo(&option);
        commandLine.option().longName("yetanother").shortName('y').description("").bindTo(&another);
        commandLine.parse(5, std::vector<const char *>{"./app", "--value", "--option=file", "--yetanother", "10"}.data());

        expect(value).toBe(true);
        expect(option).toBe("file");
        expect(another).toBe(10);
    });

    test("parse (single short name option)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        bool value = false;

        commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data());

        expect(value).toBe(true);
    });

    test("parse (multiple short name options)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        bool value = false;
        std::string option;
        std::int64_t another = 0;

        commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
        commandLine.option().longName("option").shortName('o').description("").bindTo(&option);
        commandLine.option().longName("yetanother").shortName('y').description("").bindTo(&another);
        commandLine.parse(5, std::vector<const char *>{"./app", "-v", "-o=file", "-y", "10"}.data());

        expect(value).toBe(true);
        expect(option).toBe("file");
        expect(another).toBe(10);
    });

    test("parse (mixed short and long options)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        bool value = false;
        std::string option;
        std::int64_t another = 0;
        std::string positional;

        commandLine.option().longName("value").shortName('v').description("").bindTo(&value);
        commandLine.option().longName("option").shortName('o').description("").bindTo(&option);
        commandLine.option().longName("yetanother").description("").bindTo(&another);
        commandLine.option().positional().description("").bindTo(&positional);
        commandLine.parse(6, std::vector<const char *>{"./app", "-v", "-o=file", "--yetanother", "10", "somefile"}.data());

        expect(value).toBe(true);
        expect(option).toBe("file");
        expect(another).toBe(10);
        expect(positional).toBe("somefile");
    });

    test("parse (repeated named options)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        std::vector<double> values;

        commandLine.option().longName("value").shortName('v').description("").bindTo(&values);
        commandLine.parse(7, std::vector<const char *>{"./app", "-v", "33.3", "--value=51.61234", "-v=9.999", "-v", "19.19"}.data());

        expect(values).toBe(std::vector{33.3, 51.61234, 9.999, 19.19});
    });

    test("parse (repeated positional options)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        std::vector<std::string> values;
        commandLine.option().positional().defaultValue(std::vector<std::string>{"input"}).description("").bindTo(&values);
        commandLine.parse(4, std::vector<const char *>{"./app", "input1", "input2", "input3"}.data());

        expect(values).toBe(std::vector<std::string>{"input1", "input2", "input3"});
    });

    test("parse (mixed single, missing, defaulted and repeated options)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        std::vector<int64_t> parameters;
        bool flag = true;
        std::string output;
        std::vector<std::string> paths;

        commandLine.option().longName("param").shortName('p').description("").bindTo(&parameters);
        commandLine.option().longName("flag").shortName('f').description("").bindTo(&flag);
        commandLine.option().positional().required().description("").bindTo(&output);
        commandLine.option().longName("include").shortName('I').defaultValue(std::vector<std::string>{"\"c:/path/with spaces/", "//"}).description("").bindTo(&paths);
        commandLine.parse(6, std::vector<const char *>{"a", "-p=1", "-p=2", "--param", "100", "output_option"}.data());

        expect(parameters).toBe(std::vector<std::int64_t>{1, 2, 100});
        expect(flag).toBe(true);
        expect(output).toBe("output_option");
        expect(paths).toBe(std::vector<std::string>{"\"c:/path/with spaces/", "//"});
    });

    test("parse (no arguments)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        expect([&] {
            commandLine.parse(0, std::vector<const char *>{"./app", "-v"}.data());
        })
            .toThrow<std::logic_error>("Missing mandatory first command line argument");
    });

    test("parse (unmatched argument)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data());
        })
            .toThrow<std::runtime_error>("Unknown option '-v'");
    });

    test("parse (unmatched required option)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        bool value = false;
        double required = 0.0;

        commandLine.option().longName("longName").shortName('v').description("").bindTo(&value);
        commandLine.option().positional().required().description("").bindTo(&required);

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data());
        })
            .toThrow<std::runtime_error>("Option '[positional]' was set as required but did not match any arguments");
    });

    test("parse (type mismatch)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        std::int64_t value = 0;
        commandLine.option().longName("value").shortName('v').description("").bindTo(&value);

        const std::string exceptionText = "Failed to set option 'value' (" + std::string{typeid(std::int64_t).name()} + ") from value 'hello'";

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "-v=hello"}.data());
        })
            .toThrow<std::runtime_error>(exceptionText);
    });

    test("parse (missing value)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        std::int64_t value = 0;
        commandLine.option().longName("value").shortName('v').description("").bindTo(&value);

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data());
        })
            .toThrow<std::runtime_error>("Missing value for option 'value'");
    });

    test("parse (required unmatched repeated value)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        std::vector<std::string> value;
        commandLine.option().positional().required().description("").bindTo(&value);

        expect([&] {
            commandLine.parse(1, std::vector<const char *>{"app"}.data());
        })
            .toThrow<std::runtime_error>("Option '[positional]' was set as required but did not match any arguments");
    });

    test("parse (parsing error)", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        std::int64_t value = 0;
        const std::string exceptionText = "Failed to set option 'value' (" + std::string{typeid(std::int64_t).name()} + ") from value 'hello'";

        commandLine.option().longName("value").shortName('v').description("").bindTo(&value);

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "-v=hello"}.data());
        })
            .toThrow<std::runtime_error>(exceptionText);

        const std::string expected = "ERROR: parsing command line arguments: " + exceptionText + "\nUse -? to list the command line options.\n";

        expect(stream.str()).toBe(expected);
    });

    test("setPrintStream", [] {
        std::stringstream stream;
        acore::CommandLine commandLine;
        commandLine.setPrintStream(&stream);

        commandLine.parse(2, std::vector<const char *>{"app", "-?"}.data());

        expect(stream.str()).toBe(std::string{}).toFail();
    });

    test("copy assignment", [] {
        std::int64_t value = 0;
        acore::CommandLine commandLine;
        commandLine.option().positional().description("").bindTo(&value);
        acore::CommandLine other;
        other = commandLine;
        other.parse(2, std::vector<const char *>{"./app.exe", "10"}.data());

        expect(value).toBe(10);

        commandLine.parse(2, std::vector<const char *>{"./diff", "100"}.data());

        expect(value).toBe(100);
    });

    test("move assignment", [] {
        std::int64_t value = 0;
        acore::CommandLine commandLine;
        commandLine.option().positional().description("").bindTo(&value);
        acore::CommandLine other;
        other = std::move(commandLine);
        other.parse(2, std::vector<const char *>{"./app.exe", "10"}.data());

        expect(value).toBe(10);
    });

    test("example", [] {
        const int argc = 6;
        std::array<const char *, 6> args{"./app.exe", "-i", "main.cpp", "--flag", "--Include=/some/path", "--Include=\"other/path/with space/\""}; //NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        const char **argv = args.data();

        // clang-format off
//! [[Usage]]
//int main(int argc, char *argv[])
//argc: "./app.exe", "-i", "main.cpp", "--flag", "--Include=/some/path", "--Include=\"other/path/with space/\"
std::string input;
bool flag = false;
std::string output;
std::vector<std::string> includePaths;

acore::CommandLine commandLine;
commandLine.option().longName("input").shortName('i').required().description("The required input file").bindTo(&input);
commandLine.option().longName("flag").description("An optional flag.").bindTo(&flag);
commandLine.option().positional().defaultValue(std::string{"a.o"}).description("An optional defaulted output file.").bindTo(&output);
commandLine.option().longName("Include").description("Optional repeatable argument").bindTo(&includePaths);
commandLine.parse(argc, argv);

//intput == main.cpp
//flag == true
//output == a.o
//includePaths == {/some/path, other/path/with space/}
//! [[Usage]]
        // clang-format on

        expect(input).toBe("main.cpp");
        expect(flag).toBe(true);
        expect(output).toBe("a.o");
        expect(includePaths).toBe(std::vector<std::string>{"/some/path", "other/path/with space/"});
    });
});
