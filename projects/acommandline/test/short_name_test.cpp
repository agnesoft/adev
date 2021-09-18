import atest;
import acommandline;

using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("short name", [] {
    test("single", [] {
        ::acommandline::CommandLine commandLine;

        bool value = false;

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data());

        expect(value).to_be(true);
    });

    test("multiple", [] {
        ::acommandline::CommandLine commandLine;

        bool value = false;
        std::string option;
        std::int64_t another = 0;

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);
        commandLine.option().long_name("option").short_name('o').description("").bind_to(&option);
        commandLine.option().long_name("yetanother").short_name('y').description("").bind_to(&another);
        commandLine.parse(5, std::vector<const char *>{"./app", "-v", "-o=file", "-y", "10"}.data());

        expect(value).to_be(true);
        expect(option).to_be("file");
        expect(another).to_be(10);
    });

    test("invalid (number)", [] {
        ::acommandline::CommandLine commandLine;

        expect([&] {
            static_cast<void>(commandLine.option().long_name("longName1").short_name('1'));
        })
            .to_throw<std::runtime_error>("Short name '1' is not a valid name ([a-zA-Z]).");
    });

    test("missing value", [] {
        ::acommandline::CommandLine commandLine;

        std::int64_t value = 0;
        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "-v"}.data());
        })
            .to_throw<std::runtime_error>("Missing value for option 'value'.");
    });

    test("type mismatch", [] {
        ::acommandline::CommandLine commandLine;

        std::int64_t value = 0;
        const std::string exceptionText = "Failed to set option 'value' (" + std::string{typeid(std::int64_t).name()} + ") from value 'hello'.";

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);

        expect([&] {
            commandLine.parse(2, std::vector<const char *>{"./app", "-v=hello"}.data());
        })
            .to_throw<std::runtime_error>(exceptionText);
    });
});
