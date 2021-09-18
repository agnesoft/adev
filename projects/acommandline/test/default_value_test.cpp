import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("default value", [] {
    test("named", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::string output;
        commandLine.option().long_name("long_name1").default_value("test value").description("").bind_to(&output);
        commandLine.parse(1, std::array<const char *, 1>{"./app"}.data());

        expect(output).to_be("test value");
    });

    test("positional", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::string output;
        commandLine.option().positional().default_value("test value").description("").bind_to(&output);
        commandLine.parse(1, std::array<const char *, 1>{"./app"}.data());

        expect(output).to_be("test value");
    });

    test("type mismatch", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::string output;

        expect([&] {
            commandLine.option().positional().default_value(std::int64_t{10}).description("").bind_to(&output);
        })
            .to_throw<std::runtime_error>(std::string{"The option '[positional]' default value is set with incompatible type ("} + typeid(std::int64_t).name() + ") to the one it is being bound to (" + typeid(std::string).name() + ").");
    });
});
