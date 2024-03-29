import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("positional", [] { // NOLINT(cert-err58-cpp)
    test("single", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        std::string value;

        commandLine.option().positional().description("").bind_to(&value);
        expect(commandLine.parse(2, std::array<const char *, 2>{"./app.exe", "value"}.data())).to_be(true);

        expect(value).to_be("value");
    });

    test("multiple", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::string sValue;
        std::int64_t iValue = 0;
        double dValue = 0.0;

        commandLine.option().positional().description("").bind_to(&iValue);
        commandLine.option().positional().description("").bind_to(&dValue);
        commandLine.option().positional().description("").bind_to(&sValue);
        expect(commandLine.parse(4, std::array<const char *, 4>{"./app", "value", "-1", "5.5"}.data())).to_be(true);

        expect(sValue).to_be("value");
        expect(iValue).to_be(-1);
        expect(dValue).to_be(5.5); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    });

    test("missing bound variable", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        static_cast<void>(commandLine.option().long_name("long").description(""));

        expect([&] {
            static_cast<void>(commandLine.parse(2, std::array<const char *, 2>{"./app", "--long=hello"}.data()));
        })
            .to_throw<std::runtime_error>("Unable to parse command line arguments: bind value undefined for option 'long'.\nUse -? to list the command line options.\n");
    });
});
