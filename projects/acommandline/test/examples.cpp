import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("examples", [] { // NOLINT(cert-err58-cpp)
    test("CommandLine", [] {
        const int argc = 8; // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        std::array<const char *, argc> args{"./app.exe",
                                            "-i",
                                            "main.cpp",
                                            "--flag",
                                            "--Include=/some/path",
                                            "--Include",
                                            "\"other/path/with space/\"",
                                            "-f=15.59997"};
        const char **argv = args.data();
        // clang-format off
//! [[usage]]
//int main(int argc, char *argv[])
//called as:
//  ./app.exe -i main.cpp --flag --Include=/some/path --Include "other/path/with space/" -f=15.59997

std::string input;
bool flag = false;
std::string output;
std::vector<std::string> includePaths;
double fraction = 0.0;

::acommandline::CommandLine commandLine;
commandLine.option().long_name("input").short_name('i').required().description("The required input file").bind_to(&input);
commandLine.option().long_name("flag").description("An optional flag.").bind_to(&flag);
commandLine.option().positional().default_value(std::string{"a.o"}).description("An optional defaulted output file.").bind_to(&output);
commandLine.option().long_name("Include").description("Optional repeatable argument").bind_to(&includePaths);
commandLine.option().long_name("fraction").short_name('f').description("A double value").bind_to(&fraction);
if (commandLine.parse(argc, argv))
{
    //run your application with:
    //
    //intput == "main.cpp"
    //flag == true
    //output == "a.o"
    //includePaths == {"/some/path", "other/path/with space/"}
    //fraction == 15.9997
}
else
{
    //help was displayed, probably time to exit
}
//! [[usage]]
        // clang-format on

        expect(input).to_be("main.cpp");
        expect(flag).to_be(true);
        expect(output).to_be("a.o");
        expect(includePaths).to_be(std::vector<std::string>{"/some/path", "other/path/with space/"});
        expect(fraction).to_be(15.59997); // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    });
});
