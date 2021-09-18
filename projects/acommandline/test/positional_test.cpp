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
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        std::string value;

        commandLine.option().positional().description("").bind_to(&value);
        commandLine.parse(2, std::array<const char *, 2>{"./app.exe", "value"}.data());

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
        commandLine.parse(4, std::array<const char *, 4>{"./app", "value", "-10", "5.5"}.data());

        expect(sValue).to_be("value");
        expect(iValue).to_be(-10);
        expect(dValue).to_be(5.5);
    });

    test("missing bound variable", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        static_cast<void>(commandLine.option().long_name("long").description(""));

        expect([&] {
            commandLine.parse(2, std::array<const char *, 2>{"./app", "--long=hello"}.data());
        })
            .to_throw<std::runtime_error>("Bind value undefined for option 'long'.");
    });
});
