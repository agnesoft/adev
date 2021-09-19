import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto s = suite("long name", [] { //NOLINT(cert-err58-cpp)
    test("'value'", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        bool value = false;

        commandLine.option().long_name("value").description("").bind_to(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "--value"}.data());

        expect(value).to_be(true);
    });

    test("'long.name'", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        std::int64_t value = 0;

        commandLine.option().long_name("long.name").description("").bind_to(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "--long.name=3"}.data());

        expect(value).to_be(3);
    });

    test("'long-name'", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        std::int64_t value = 0;

        commandLine.option().long_name("long-name").description("").bind_to(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "--long-name=-2"}.data());

        expect(value).to_be(-2);
    });

    test("'long_name'", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        std::int64_t value = 0;

        commandLine.option().long_name("long_name").description("").bind_to(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "--long_name=-1"}.data());

        expect(value).to_be(-1);
    });

    test("'longName'", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        std::int64_t value = 0;

        commandLine.option().long_name("longName").description("").bind_to(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "--longName=3"}.data());

        expect(value).to_be(3);
    });

    test("'Long--Name.1'", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        std::int64_t value = 0;

        commandLine.option().long_name("Long--Name.1").description("").bind_to(&value);
        commandLine.parse(2, std::vector<const char *>{"./app", "--Long--Name.1=4"}.data());

        expect(value).to_be(4);
    });

    test("multiple", [] {
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};

        bool value = false;
        std::string option;
        std::int64_t another = 0;
        constexpr int argc = 5;

        commandLine.option().long_name("value").description("").bind_to(&value);
        commandLine.option().long_name("option").description("").bind_to(&option);
        commandLine.option().long_name("yetanother").short_name('y').description("").bind_to(&another);
        commandLine.parse(argc, std::array<const char *, argc>{"./app", "--value", "--option=file", "--yetanother", "-1"}.data());

        expect(value).to_be(true);
        expect(option).to_be("file");
        expect(another).to_be(-1);
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
