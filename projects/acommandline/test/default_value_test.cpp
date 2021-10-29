import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("default value", [] { // NOLINT(cert-err58-cpp)
    test("named", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::string output;
        commandLine.option().long_name("long_name1").default_value("test value").description("").bind_to(&output);
        expect(commandLine.parse(1, std::array<const char *, 1>{"./app"}.data())).to_be(true);

        expect(output).to_be("test value");
    });

    test("positional", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::string output;
        commandLine.option().positional().default_value("test value").description("").bind_to(&output);
        expect(commandLine.parse(1, std::array<const char *, 1>{"./app"}.data())).to_be(true);

        expect(output).to_be("test value");
    });

    test("type mismatch", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::string output;

        expect([&] {
            commandLine.option().positional().default_value(std::int64_t{3}).description("").bind_to(&output);
        })
            .to_throw<std::logic_error>(std::string{"The option '[positional]' default value is set with incompatible type ("} + typeid(std::int64_t).name() + ") to the one it is being bound to (" + typeid(std::string).name() + ").");
    });

    test("missing bound variable", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        static_cast<void>(commandLine.option().positional().default_value(std::int64_t{3}).description(""));

        expect([&] {
            static_cast<void>(commandLine.parse(1, std::array<const char *, 1>{"./app"}.data()));
        })
            .to_throw<std::runtime_error>(std::string{"Unable to parse command line arguments: bind value undefined for option '[positional]'.\nUse -? to list the command line options.\n"});
    });
});
