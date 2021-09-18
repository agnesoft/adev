import atest;
import acommandline;

using ::atest::assert_;
using ::atest::assert_fail;
using ::atest::expect;
using ::atest::expect_fail;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("long name", [] {
    test("single", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        bool value = false;

        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "--value"}.data());

        expect(value).to_be(true);
    });

    test("multiple", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        bool value = false;
        std::string option;
        std::int64_t another = 0;

        commandLine.option().long_name("value").description("").bind_to(&value);
        commandLine.option().long_name("option").description("").bind_to(&option);
        commandLine.option().long_name("yetanother").short_name('y').description("").bind_to(&another);
        commandLine.parse(5, std::array<const char *, 5>{"./app", "--value", "--option=file", "--yetanother", "10"}.data());

        expect(value).to_be(true);
        expect(option).to_be("file");
        expect(another).to_be(10);
    });

    test("type mismatch", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        std::int64_t value = 0;
        commandLine.option().long_name("value").short_name('v').description("").bind_to(&value);

        const std::string exceptionText = "Failed to set option 'value' (" + std::string{typeid(std::int64_t).name()} + ") from value 'hello'.";

        expect([&] {
            commandLine.parse(2, std::array<const char *, 2>{"./app", "-v=hello"}.data());
        })
            .to_throw<std::runtime_error>(exceptionText);
    });
});
