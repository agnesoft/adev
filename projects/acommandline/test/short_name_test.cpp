import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("short name", [] { //NOLINT(cert-err58-cpp)
    test("'S'", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        bool value = false;

        commandLine.option().long_name("short").short_name('S').description("").bind_to(&value);
        commandLine.parse(2, std::array<const char *, 2>{"./app", "-S"}.data());

        expect(value).to_be(true);
    });

    test("'s'", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::int64_t value = 0;

        commandLine.option().long_name("short").short_name('s').description("").bind_to(&value);
        commandLine.parse(3, std::array<const char *, 3>{"./app", "-s", "3"}.data());

        expect(value).to_be(3);
    });

    test("multiple", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        bool value = false;
        std::string option;
        std::int64_t another = 0;
        constexpr int argc = 5;

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);
        commandLine.option().long_name("option").short_name('o').description("").bind_to(&option);
        commandLine.option().long_name("yetanother").short_name('y').description("").bind_to(&another);
        commandLine.parse(argc, std::array<const char *, argc>{"./app", "-v", "-o=file", "-y", "-4"}.data());

        expect(value).to_be(true);
        expect(option).to_be("file");
        expect(another).to_be(-4);
    });

    test("invalid (number)", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        expect([&] {
            static_cast<void>(commandLine.option().long_name("longName1").short_name('1'));
        })
            .to_throw<std::runtime_error>("'1' is not a valid option short name ([a-zA-Z]).");
    });

    test("missing value", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::int64_t value = 0;
        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);

        expect([&] {
            commandLine.parse(2, std::array<const char *, 2>{"./app", "-v"}.data());
        })
            .to_throw<std::runtime_error>("Missing value for option 'value'.");
    });

    test("type mismatch", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::int64_t value = 0;
        const std::string exceptionText = "Failed to set option 'value' (" + std::string{typeid(std::int64_t).name()} + ") from value 'hello'.";

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);

        expect([&] {
            commandLine.parse(2, std::array<const char *, 2>{"./app", "-v=hello"}.data());
        })
            .to_throw<std::runtime_error>(exceptionText);
    });
});
