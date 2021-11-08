import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("wait", [] { // NOLINT(cert-err58-cpp)
    test("immediate job", [] {
        aprocess::Process process{{.command = "aprocesstestapp"}};
        expect(process.is_running()).to_be(true);
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("short job", [] {
        aprocess::Process process{{.command = "aprocesstestapp",
                                   .arguments = {"--echo=Hi", "--echo-delay=10"}}};
        expect(process.is_running()).to_be(true);
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("long job", [] {
        aprocess::Process process{{.command = "aprocesstestapp",
                                   .arguments = {"--echo=Hi", "--echo-delay=100"}}};
        expect(process.is_running()).to_be(true);

        expect([&] {
            process.wait(std::chrono::milliseconds{10});
        })
            .to_throw<std::runtime_error>("Wait for process timed out.");

        expect(process.is_running()).to_be(true);
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("stopped job", [] {
        aprocess::Process process{{.command = "aprocesstestapp"}};
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        assert_(process.is_running()).to_be(false);
        process.wait(std::chrono::milliseconds{10});
    });
});
