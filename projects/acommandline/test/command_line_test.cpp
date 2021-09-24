import atest;
import acommandline;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static const auto S = suite("CommandLine", [] { //NOLINT(cert-err58-cpp)
    test("default constructible", [] {
        expect(std::is_default_constructible_v<::acommandline::CommandLine>).to_be(true);
    });

    test("copy constructible", [] {
        expect(std::is_copy_constructible_v<::acommandline::CommandLine>).to_be(true);
    });

    test("nothrow move constructible", [] {
        expect(std::is_nothrow_move_constructible_v<::acommandline::CommandLine>).to_be(true);
    });

    test("nothrow destructible", [] {
        expect(std::is_nothrow_destructible_v<::acommandline::CommandLine>).to_be(true);
    });

    test("constructor", [] {
        ::acommandline::CommandLine{};
    });

    test("copy constructor", [] {
        std::int64_t value = 0;
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        commandLine.option().positional().description("").bind_to(&value);
        ::acommandline::CommandLine other{commandLine};
        other.parse(2, std::array<const char *, 2>{"./app.exe", "4"}.data());

        expect(value).to_be(4);

        commandLine.parse(2, std::array<const char *, 2>{"./diff", "-3"}.data());

        expect(value).to_be(-3);
    });

    test("move constructor", [] {
        std::int64_t value = 0;
        std::stringstream stream;
        ::acommandline::CommandLine commandLine{stream};
        commandLine.option().positional().description("").bind_to(&value);
        ::acommandline::CommandLine other{std::move(commandLine)};
        other.parse(2, std::array<const char *, 2>{"./app.exe", "4"}.data());

        expect(value).to_be(4);
    });
});
