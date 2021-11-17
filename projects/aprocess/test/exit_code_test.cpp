import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("exit code", [] { // NOLINT(cert-err58-cpp)
    test("0", [] {
        expect(::aprocess::create_process()
                   .command("aprocesstestapp")
                   .wait(DEFAULT_WAIT_TIMEOUT)
                   .exit_code())
            .to_be(0);
    });

    test("1", [] {
        expect(::aprocess::create_process()
                   .command("aprocesstestapp")
                   .arg("--exit-code=1")
                   .wait(DEFAULT_WAIT_TIMEOUT)
                   .exit_code())
            .to_be(1);
    });

    test("-1", [] {
        expect(::aprocess::create_process()
                   .command("aprocesstestapp")
                   .arg("--exit-code=-1")
                   .wait(DEFAULT_WAIT_TIMEOUT)
                   .exit_code())
            .to_be(-1);
    });
});
