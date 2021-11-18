import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

namespace aprocess
{
[[nodiscard]] auto operator==(const ::aprocess::EnvironmentVariable &left, const ::aprocess::EnvironmentVariable &right) -> bool
{
    return left.name == right.name && left.value == right.value;
}

auto operator<<(std::ostream &stream, const ::aprocess::EnvironmentVariable &variable) -> std::ostream &
{
    stream << variable.name << "=" << variable.value;
    return stream;
}
}

static const auto S = suite("output", [] { // NOLINT(cert-err58-cpp)
    test("no output", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{});
        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{});
        expect(output).to_be(std::string{});
    });

    test("Hello, World!", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arguments({"--echo=Hello", "--echo=,", "--echo=World", "--echo=!"})
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{"--echo=Hello", "--echo=,", "--echo=World", "--echo=!"});
        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{});
        expect(output).to_be(std::string{"Hello,World!"});
    });

    test("large output", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arguments({"--echo-large=A", "--echo-large-size=1000000"})
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{"--echo-large=A", "--echo-large-size=1000000"});
        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{});

        constexpr std::size_t LARGE_SIZE = 1'000'000;
        assert_(output.size()).to_be(LARGE_SIZE);
        expect(output == std::string(LARGE_SIZE, 'A')).to_be(true);
    });

    test("environment variable", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arguments({"--echo-env=aprocesstest_envvar"})
                                          .env({"aprocesstest_envvar", "somevalue1"})
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{"--echo-env=aprocesstest_envvar"});
        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{{"aprocesstest_envvar", "somevalue1"}});
        expect(output).to_be(std::string{"somevalue1"});
    });

    test("environment variables", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-env=aprocesstestenvvar")
                                          .arg("--echo-env=aprocesstestenvvar2")
                                          .environment({{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}})
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{"--echo-env=aprocesstestenvvar", "--echo-env=aprocesstestenvvar2"});
        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}});
        expect(output).to_be(std::string{"somevalue1somevalue2"});
    });

    test("PATH", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-env=PATH")
                                          .environment({{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}})
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{"--echo-env=PATH"});
        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}});
        expect(output.empty()).to_be(false);
    });

    test("working directory (default)", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-working-directory")
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{"--echo-working-directory"});
        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{});
        expect(process.working_directory()).to_be(std::filesystem::current_path().string());
        expect(output).to_be(std::filesystem::current_path().string());
    });

    test("working directory (custom)", [] {
        std::string output;
        const std::filesystem::path dir = std::filesystem::canonical(std::filesystem::current_path().root_directory());

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-working-directory")
                                          .working_directory(dir.string())
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{"--echo-working-directory"});
        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{});

        expect(process.working_directory()).to_be(dir.string());
        expect(output).to_be(dir.string());
    });
});
