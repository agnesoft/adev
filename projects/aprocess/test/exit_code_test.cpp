import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("exit code", [] { // NOLINT(cert-err58-cpp)
    test("0", [] {
        aprocess::Process process{{.command = "aprocesstestapp"}};
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.exit_code()).to_be(0);
    });

    test("1", [] {
        aprocess::Process process{{.command = "aprocesstestapp",
                                   .arguments{"--exit-code", "1"}}};
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.exit_code()).to_be(1);
    });

    test("-1", [] {
        aprocess::Process process{{.command = "aprocesstestapp",
                                   .arguments{"--exit-code", "-1"}}};
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.exit_code()).to_be(-1);
    });
});
