import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{2000};

static const auto S = suite("terminate", [] { // NOLINT(cert-err58-cpp)
    test("running process", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arg("--echo-input");

        assert_(process.is_running()).to_be(true);
        process.terminate();
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("stopped process", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp");

        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
        process.terminate();
        expect(process.is_running()).to_be(false);
    });
});
