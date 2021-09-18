import atest;
import acommandline;

using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("positional", [] {
    test("single", [] {
        ::acommandline::CommandLine commandLine;
        std::string value;

        commandLine.option().positional().description("").bind_to(&value);
        commandLine.parse(2, std::vector<const char *>{"./app.exe", "value"}.data());

        expect(value).to_be("value");
    });

    test("multiple", [] {
        ::acommandline::CommandLine commandLine;

        std::string sValue;
        std::int64_t iValue = 0;
        double dValue = 0.0;

        commandLine.option().positional().description("").bind_to(&iValue);
        commandLine.option().positional().description("").bind_to(&dValue);
        commandLine.option().positional().description("").bind_to(&sValue);
        commandLine.parse(4, std::vector<const char *>{"./app", "value", "-10", "5.5"}.data());

        expect(sValue).to_be("value");
        expect(iValue).to_be(-10);
        expect(dValue).to_be(5.5);
    });

    test("type mismatch", [] {
        ::acommandline::CommandLine commandLine;

        std::int64_t value = 0;
        commandLine.option().positional().description("").bind_to(&value);

        const std::string exceptionText = "Failed to set option '[positional]' (" + std::string{typeid(std::int64_t).name()} + ") from value 'hello'.";

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "hello"}.data());
        })
            .to_throw<std::runtime_error>(exceptionText);
    });

    test("missing bound variable", [] {
        ::acommandline::CommandLine commandLine;
        static_cast<void>(commandLine.option().long_name("long").description(""));

        const std::string exceptionText = "Undefined bound variable for option '[positional]'.";

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "long=hello"}.data());
        })
            .to_throw<std::runtime_error>(exceptionText);
    });
});
