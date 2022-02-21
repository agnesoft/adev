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

static constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("environment", [] { // NOLINT(cert-err58-cpp)
    test("variable", [] {
        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arguments({"--echo-env=aprocesstest_envvar"})
                                          .env({"aprocesstest_envvar", "somevalue1"})
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{{"aprocesstest_envvar", "somevalue1"}});
        expect(process.read()).to_be("somevalue1");
    });

    test("variables", [] {
        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-env=aprocesstestenvvar")
                                          .arg("--echo-env=aprocesstestenvvar2")
                                          .environment({{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}})
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}});
        expect(process.read()).to_be("somevalue1somevalue2");
    });

    test("PATH", [] {
        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-env=PATH")
                                          .environment({{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}})
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.environment()).to_be(std::vector<::aprocess::EnvironmentVariable>{{"aprocesstestenvvar", "somevalue1"}, {"aprocesstestenvvar2", "somevalue2"}});
        expect(process.read().empty()).to_be(false);
    });
});
