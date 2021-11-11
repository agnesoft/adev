import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{100};

static const auto S = suite("terminate", [] { // NOLINT(cert-err58-cpp)
    test("stoppable process", [] {
        aprocess::Process process{{.command = "aprocesstestapp",
                                   .arguments = {"--echo-input", "--stoppable"}}};
        assert_(process.is_running()).to_be(true);
        process.terminate();
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("unstoppable process", [] {
        aprocess::Process process{{.command = "aprocesstestapp",
                                   .arguments = {"--echo-input"}}};
        expect(process.is_running()).to_be(true);
        process.terminate();

        expect([&] {
            process.wait(std::chrono::milliseconds{100});
        })
            .to_throw<std::runtime_error>("Wait for process timed out.");

        expect(process.is_running()).to_be(true);
    });

    test("stopped process", [] {
        aprocess::Process process{{.command = "aprocesstestapp"}};
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
        process.terminate();
        expect(process.is_running()).to_be(false);
    });
});
