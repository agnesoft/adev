import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

auto is_process_running(std::int64_t pid) -> bool;
static constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("kill", [] { // NOLINT(cert-err58-cpp)
    test("running process", [] {
        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .arg("--echo-input");

        assert_(process.is_running()).to_be(true);
        process.kill();
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("stopped process", [] {
        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp");

        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        assert_(process.is_running()).to_be(false);
        process.kill();
    });

    test("destructor kills process", [] {
        std::int64_t pid = 0;

        {
            ::aprocess::Process process = ::aprocess::create_process()
                                              .command("aprocesstestapp")
                                              .arguments({"--echo-input", "--echo-input-timeout=10000"});

            assert_(process.is_running()).to_be(true);
            pid = process.pid();
        }

        const auto end = std::chrono::system_clock::now() + DEFAULT_WAIT_TIMEOUT;

        while (std::chrono::system_clock::now() < end && ::is_process_running(pid))
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds{1});
        }

        expect(::is_process_running(pid)).to_be(false);
    });
});
