import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

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

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("environment", [] { // NOLINT(cert-err58-cpp)
    test("variable", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arguments({"--echo-env=aprocesstest_envvar"})
                                          .env({"aprocesstest_envvar", "somevalue1"})
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{{"aprocesstest_envvar", "somevalue1"}});
        expect(output).to_be(std::string{"somevalue1"});
    });

    test("variables", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-env=aprocesstestenvvar")
                                          .arg("--echo-env=aprocesstestenvvar2")
                                          .environment({{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}})
                                          .read([&](std::string_view message) { output += message; })
                                          .wait(DEFAULT_WAIT_TIMEOUT);

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

        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}});
        expect(output.empty()).to_be(false);
    });
});
