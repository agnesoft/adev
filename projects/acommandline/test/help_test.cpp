import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("help", [] { //NOLINT(cert-err58-cpp)
    test("Mixed options", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        bool b = false;
        std::int64_t i{};
        std::string str;
        double d{};

        commandLine.option().long_name("long_name1").short_name('l').description("Some long name argument.").bind_to(&b);
        commandLine.option().long_name("another").default_value(13.3).description("Another argument description.").bind_to(&d); //NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        commandLine.option().positional().required().description("Positional argument description.").bind_to(&str);
        commandLine.option().long_name("output").short_name('o').description("Output parameter.").bind_to(&i);
        commandLine.parse(2, std::array<const char *, 2>{"app", "-?"}.data());

        const std::string expected = "Usage:\n"
                                     "    app [options]\n"
                                     "Syntax:\n"
                                     "  switch       --switch, -s\n"
                                     "  named        --name=value, --n=value, --name value, --n value\n"
                                     "  positional   value\n"
                                     "Options:\n"
                                     "    -?                   [switch]            Prints this help.\n"
                                     "  Required:\n"
                                     "    [positional]         [string]            Positional argument description.\n"
                                     "  Optional:\n"
                                     "    -l, --long_name1     [switch]            Some long name argument.\n"
                                     "    --another            [floating=13.3]     Another argument description.\n"
                                     "    -o, --output         [integer]           Output parameter.\n"
                                     "  Positional:\n"
                                     "    [positional]         [string]            Positional argument description.\n";

        expect(stream.str()).to_be(expected);
    });
});
