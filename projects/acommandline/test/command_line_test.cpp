import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("CommandLine", [] {
    test("default constructible", [] {
        expect(std::is_default_constructible_v<::acommandline::CommandLine>).to_be(true);
    });

    test("copy constructible", [] {
        expect(std::is_copy_constructible_v<::acommandline::CommandLine>).to_be(true);
    });

    test("nothrow move constructible", [] {
        expect(std::is_nothrow_move_constructible_v<::acommandline::CommandLine>).to_be(true);
    });

    test("nothrow destructible", [] {
        expect(std::is_nothrow_destructible_v<::acommandline::CommandLine>).to_be(true);
    });

    test("constructor", [] {
        ::acommandline::CommandLine{};
    });

    test("copy constructor", [] {
        std::int64_t value = 0;
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        commandLine.option().positional().description("").bind_to(&value);
        ::acommandline::CommandLine other{commandLine};
        other.parse(2, std::array<const char *, 2>{"./app.exe", "10"}.data());

        expect(value).to_be(10);

        commandLine.parse(2, std::array<const char *, 2>{"./diff", "100"}.data());

        expect(value).to_be(100);
    });

    test("move constructor", [] {
        std::int64_t value = 0;
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        commandLine.option().positional().description("").bind_to(&value);
        ::acommandline::CommandLine other{std::move(commandLine)};
        other.parse(2, std::array<const char *, 2>{"./app.exe", "10"}.data());

        expect(value).to_be(10);
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

::acommandline::CommandLine commandLine;
commandLine.option().long_name("input").short_name('i').required().description("The required input file").bind_to(&input);
commandLine.option().long_name("flag").description("An optional flag.").bind_to(&flag);
commandLine.option().positional().default_value(std::string{"a.o"}).description("An optional defaulted output file.").bind_to(&output);
commandLine.option().long_name("Include").description("Optional repeatable argument").bind_to(&includePaths);
commandLine.parse(argc, argv);

//intput == main.cpp
//flag == true
//output == a.o
//includePaths == {/some/path, other/path/with space/}
//! [[Usage]]
        // clang-format on

        expect(input).to_be("main.cpp");
        expect(flag).to_be(true);
        expect(output).to_be("a.o");
        expect(includePaths).to_be(std::vector<std::string>{"/some/path", "other/path/with space/"});
    });
});
