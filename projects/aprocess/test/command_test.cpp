import atest;
import aprocess;

using ::atest::expect;
using ::atest::suite;
using ::atest::test;

static constexpr std::chrono::milliseconds DEFAULT_WAIT_TIMEOUT{1000};

static const auto S = suite("command", [] { // NOLINT(cert-err58-cpp)
    test("program name", [] {
        std::string output;

        ::aprocess::Process process = ::aprocess::create_process()
                                          .command("aprocesstestapp")
                                          .wait(DEFAULT_WAIT_TIMEOUT);

        expect(process.command()).to_be("aprocesstestapp");
    });

    test("empty name", [] {
        expect([] { ::aprocess::create_process()
                        .command("")
                        .wait(DEFAULT_WAIT_TIMEOUT); })
            .to_throw<std::runtime_error>();
    });

    test("unknown program", [] {
        expect([] { ::aprocess::create_process()
                        .command("aprocesstestapp2")
                        .wait(DEFAULT_WAIT_TIMEOUT); })
            .to_throw<std::runtime_error>();
    });
});
