import atest;
import aprocess;

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

auto is_process_running(std::int64_t pid) -> bool;
constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{20};

static const auto S = suite("detached", [] { // NOLINT(cert-err58-cpp)
    test("outlives the object", [] {
        std::int64_t pid = 0;

        {
            ::aprocess::Process process = ::aprocess::create_process()
                                              .command("aprocesstestapp")
                                              .arg("--wait=50")
                                              .detached();

            expect(process.is_running()).to_be(true);
            pid = process.pid();
        }

        std::this_thread::sleep_for(DEFAULT_WAIT_TIMEOUT);
        expect(is_process_running(pid)).to_be(true);
    });
});
