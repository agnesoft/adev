import atest;
import aprocess;

#ifdef _WIN32
import awinapi;

[[nodiscard]] auto is_process_running(std::int64_t pid) -> bool
{
    ::HANDLE process = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(pid));

    if (process != nullptr)
    {
        ::CloseHandle(process);
        return true;
    }

    return false;
}
#endif

using ::atest::assert_;
using ::atest::expect;
using ::atest::suite;
using ::atest::test;

constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{100};

static const auto S = suite("kill", [] { // NOLINT(cert-err58-cpp)
    test("stoppable process", [] {
        aprocess::Process process{{.command = "aprocesstestapp",
                                   .arguments = {"--echo-input", "--stoppable"}}};
        assert_(process.is_running()).to_be(true);
        process.kill();
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("unstoppable process", [] {
        aprocess::Process process{{.command = "aprocesstestapp",
                                   .arguments = {"--echo-input"}}};
        expect(process.is_running()).to_be(true);
        process.kill();
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
    });

    test("stopped process", [] {
        aprocess::Process process{{.command = "aprocesstestapp"}};
        process.wait(std::chrono::milliseconds{DEFAULT_WAIT_TIMEOUT});
        expect(process.is_running()).to_be(false);
        process.kill();
    });

    test("~Process() kills process", [] {
        std::int64_t pid = 0;

        {
            aprocess::Process process{{.command = "aprocesstestapp",
                                       .arguments = {"--echo-input", "--echo-input-timeout=10000"}}};
            expect(process.is_running()).to_be(true);
            pid = process.pid();
        }

        std::cout << pid << '\n';
        const auto end = std::chrono::system_clock::now() + DEFAULT_WAIT_TIMEOUT;

        while (std::chrono::system_clock::now() < end && ::is_process_running(pid))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }

        expect(::is_process_running(pid)).to_be(false);
    });
});
