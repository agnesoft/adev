import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("wait", [] { // NOLINT(cert-err58-cpp)
    test("immediate process", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp");

        assert_(process.is_running()).to_be(true);
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("short running process", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arg("--echo=Hi")
                .arg("--echo-delay=10");

        assert_(process.is_running()).to_be(true);
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("long running process", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp")
                .arg("--echo=Hi")
                .arg("--echo-delay=10");

        assert_(process.is_running()).to_be(true);

        expect([&] {
            process.wait(std::chrono::milliseconds{1});
        })
            .to_throw<std::runtime_error>("Wait for process timed out.");

        assert_(process.is_running()).to_be(true);
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("stopped process", [] {
        ::aprocess::Process process =
            ::aprocess::create_process()
                .command("aprocesstestapp");

        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        assert_(process.is_running()).to_be(false);
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
    });
});
