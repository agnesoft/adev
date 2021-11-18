import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("arguments", [] { // NOLINT(cert-err58-cpp)
    test("arg()", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo=Hello")
                                          .arg("--echo=World")
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{"--echo=Hello", "--echo=World"});
    });

    test("arguments()", [] {
        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arguments({"--echo=Hello", "--echo=World"})
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.arguments()).to_be(std::vector<std::string>{"--echo=Hello", "--echo=World"});
    });
});
